
	double factor_table[11] = {
		0,
		20.75,
		16.92,
		13.98,
		11.5,
		9.32, 
		7.34,
		5.53,
		3.84,
		2.25,
		0.75,
	};
	
	if(globalCounterParameter1 != 0){
		// Calculate effect parameter
		thresholdValue = (long)(50000 * factor_table[globalCounterParameter1]); 
		
		// Read from FIFO
		sampleValue = (long) p_mmap[PCM_FIFO_A];
		
		// perform hard clipping
		if(sampleValue > (0 + thresholdValue)){
			sampleValue= 0 + thresholdValue ;
		}
		if(sampleValue < (0 - thresholdValue)){
			sampleValue= 0 - thresholdValue;
		}
		
		// Change to get samples on the same level
		sampleValue = (long)(sampleValue* (double) 1 / factor_table[globalCounterParameter1] );
		
		// Achieve same loudness compared to other effects
		p_mmap[PCM_FIFO_A] = sampleValue<<3;
		
	} else {
		sampleValueClean = (long) p_mmap[PCM_FIFO_A];
		sampleValueClean = (long)(sampleValueClean * (double)0.385);
		p_mmap[PCM_FIFO_A] = sampleValueClean;
	}
	

