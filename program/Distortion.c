int i2sDistortionEffect(				// Rueckgabewert: EXIT_SUCCESS bei fehlerfreiem Abschluss der Funktion, sonst EXIT_FAILURE
		volatile ulong* p_mmap)// [in/out] Zeiger auf den per mmap() geteilten Speicher
{ 
	long tempLeft, tempLeftforNoDistortion;
	long tempRight,tempRightforNoDistortion;
	long distortion_Value = 50000; // found out by hearing
	double lookupTable_Factor[11] = {
		0,
		20.75, // 1
		16.92,
		13.98,
		11.5,
		9.32,  // 5
		7.34,
		5.53,
		3.84,
		2.25,
		0.75, //10
	};
	
	
	int ret = EXIT_SUCCESS;	// Rueckgabe-Wert
	ulong cnt = 0;			// Zaehl-Variable
	ulong tx_error_cnt = 0;	// Zaehl-Variable
	ulong rx_error_cnt = 0;	// Zaehl-Variable

	writeMemMaskedBit(&p_mmap[PCM_CS_A], PCM_CS_A__EN__MASK, PCM_CS_A__EN__VAL(1)); //EN
	writeMemMaskedBit(&p_mmap[PCM_CS_A], PCM_CS_A__TXCLR__MASK, PCM_CS_A__TXCLR__VAL(1)); //TX Fifo loeschen
	writeMemMaskedBit(&p_mmap[PCM_CS_A], PCM_CS_A__RXCLR__MASK, PCM_CS_A__RXCLR__VAL(1)); //RX Fifo loeschen
	waitForSyncBit(p_mmap);
	writeMemMaskedBit(&p_mmap[PCM_CS_A], PCM_CS_A__RXON__MASK, (0x1 << PCM_CS_A__RXON__BASE)); //RX ON
	//TX ON nicht hier, sondern weiter unten nach dem Fuellen des Fifo

	cnt = 0;
	//Datenweiterleitung solange, bis Benutzer abbricht
	while(!abort_flag && !chFilt) {
		
		// Calculate Effect Parameters
		distortion_Value = (long)(50000 * lookupTable_Factor[globalCounterParameter1]); // Attention will change the amplitude
		
		
		//Datenwort fuer linken Kanal aus Empfangs-Fifo auslesen und den Sende-Fifo schreiben, sofern Platz ist
		
		// Left is the only affected Channel !!!
		while ((!(p_mmap[PCM_CS_A] & PCM_CS_A__RXD__MASK)) && !abort_flag) //warte solang RX Fifo leer ist (= Bit gesetzt)
			; // warten
		
		if(globalCounterParameter1 != 0){ // else simple clean sound
		
			tempLeft = (long) p_mmap[PCM_FIFO_A];
			
			if(tempLeft > (0 + distortion_Value)){
				tempLeft= 0 + distortion_Value ;
			}
			
			if(tempLeft < (0 - distortion_Value)){
				tempLeft= 0 - distortion_Value;
			}
			
			//~ tempLeft = (long)(tempLeft* (double) 1 / lookupTable_Factor[globalCounterParameter1] );// Change to get original loudness (amplitude)

			p_mmap[PCM_FIFO_A] = tempLeft; // Oszi NOTE: with bitshift <<3, little less ampl compared to clean
			
		} else {
			tempLeftforNoDistortion = (long) p_mmap[PCM_FIFO_A];
			//~ tempLeftforNoDistortion = (long)(tempLeftforNoDistortion * (double)0.385);
			p_mmap[PCM_FIFO_A] = tempLeftforNoDistortion;
		}
		
		

		

		//Datenwort fuer rechten Kanal aus Empfangs-Fifo auslesen und den Sende-Fifo schreiben, sofern Platz ist
		while ((!(p_mmap[PCM_CS_A] & PCM_CS_A__RXD__MASK)) && !abort_flag) //warte solang RX Fifo leer ist (= Bit gesetzt)
			; //warten
			
		if(globalCounterParameter1 != 0){ // else simple clean sound
			
			tempRight = (long) p_mmap[PCM_FIFO_A];
			
			if(tempRight > (0 + distortion_Value)){
				tempRight= 0 + distortion_Value; //was 1500000 for nearly FUZZ good sound
			}
			
			if(tempRight < (0 - distortion_Value)){
				tempRight= 0 - distortion_Value; //was -1500000 for nearly FUZZ good sound
			}
			
			//~ tempRight = (long)(tempRight* (double) 1 / lookupTable_Factor[globalCounterParameter1] );// Change to get original loudness (amplitude)
			
			p_mmap[PCM_FIFO_A] =  tempRight; // Oszi NOTE: with bitshift <<3, little less ampl compared to clean
			
		} else {
			tempRightforNoDistortion = (long) p_mmap[PCM_FIFO_A];
			//~ tempRightforNoDistortion = (long)(tempRightforNoDistortion * (double)0.385);
			p_mmap[PCM_FIFO_A] = tempRightforNoDistortion;
		}
		

		// nach 2x32 Eintraegen ist Sendefifo voll und meldet Fehler bei weiterem Datum, daher bei 2x31 Eintraegen (0..30) TX aktivieren
		if(cnt == 30)
			writeMemMaskedBit(&p_mmap[PCM_CS_A], PCM_CS_A__TXON__MASK, PCM_CS_A__TXON__VAL(1)); //TX ON

		//falls der Empfangs-Fifo uebergelaufen ist: Warnung ausgeben, Stream neu starten und Rueckgabewert auf Fehler setzen
		if (p_mmap[PCM_CS_A] & PCM_CS_A__RXERR__MASK) {
			rx_error_cnt++;
			printf("RX Fifo Error... Schleife: %lu  RX Error Anzahl: %lu, Neustart.\n", cnt, rx_error_cnt);
			writeMemMaskedBit(&p_mmap[PCM_CS_A], PCM_CS_A__RXERR__MASK, PCM_CS_A__RXERR__VAL(1)); // Flag zuruecksetzen (1 schreiben)
			i2sRestartStream(p_mmap);
			cnt = 0;
			ret = EXIT_FAILURE;
		}
		//falls der Sende-Fifo leergelaufen ist: Warnung ausgeben, Stream neu starten und Rueckgabewert auf Fehler setzen
		if (p_mmap[PCM_CS_A] & PCM_CS_A__TXERR__MASK) {
			tx_error_cnt++;
			printf("TX Fifo Error... Schleife: %lu  TX Error Anzahl: %lu, Neustart.\n", cnt, tx_error_cnt);
			writeMemMaskedBit(&p_mmap[PCM_CS_A], PCM_CS_A__TXERR__MASK, PCM_CS_A__TXERR__VAL(1)); // Flag zuruecksetzen (1 schreiben)
			i2sRestartStream(p_mmap);
			cnt = 0;
			ret = EXIT_FAILURE;
		}
		cnt++;
	}
	if(chFilt){
		return EXIT_SUCCESS;
	}
	return ret;
}
