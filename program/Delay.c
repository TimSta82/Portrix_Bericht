		// Calculate effect parameters
		level = ((double)globalCounterParameter1 / 10);
		decay = ((double)globalCounterParameter2 / 10);
		time =  globalCounterParameter3* 5000;
		
		// Read from FIFO
		inputSample = (long) p_mmap[PCM_FIFO_A]; 
		
		// Fill delay fuffer and manipulate by decay factor
		delayedSample = (inputSample + delayBuffer[delayCounter]); 
		delayedSample = (long)(delayedSample* decay);		
		delayBuffer[delayCounter] = delayedSample; 
		
		// Inkrement index to cause delay
		delayCounter++;
		
		// Clip Delay counter
		if(delayCounter >= time){
			delayCounter = 0;
		}
		
		// vary gain level of delayed sample
		delayBuffer[delayCounter] = (long)(delayBuffer[delayCounter]* level);			
		outputSample = (delayBuffer[delayCounter]+inputSample);
		
		// write to FIFO
		p_mmap[PCM_FIFO_A] = outputSample;