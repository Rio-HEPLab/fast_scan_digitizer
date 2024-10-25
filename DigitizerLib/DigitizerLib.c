#include "DigitizerLib.h"

/* ###########################################################################
*  Functions
*  ########################################################################### */
/*! \fn      static long get_time()
*   \brief   Get time in milliseconds
*
*   \return  time in msec
*/
static long get_time()
{
    long time_ms;
#ifdef WIN32
    struct _timeb timebuffer;
    _ftime( &timebuffer );
    time_ms = (long)timebuffer.time * 1000 + (long)timebuffer.millitm;
#else
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;
#endif
    return time_ms;
}

/*! \fn      static double linear_interp()
*   \brief   Calculates the line between two points and return its value on the given 'x'
*
*   \param   x0   x-value of first point
*   \param   y0   y-value of first point
*   \param   x0   x-value of second point
*   \param   y0   y-value of second point
*   \param   x    x value of the return value
*   \return  the 'y' value corresponding to the given 'x'
*/
static double linear_interp(double x0, double y0, double x1, double y1, double x) {
    if (x1 - x0 == 0) {
        fprintf(stderr, "Cannot interpolate values with same x.\n");
        return HUGE_VAL;
    }
    else {
        const double m = (y1 - y0) / (x1 - x0);
        const double q = y1 - m * x1;
        return m * x + q;
    }
}

/*! \fn      int GetMoreBoardNumChannels(CAEN_DGTZ_BoardInfo_t BoardInfo,  WaveDumpConfig_t *WDcfg)
*   \brief   calculate num of channels, num of bit and sampl period according to the board type
*
*   \param   BoardInfo   Board Type
*   \param   WDcfg       pointer to the config. struct
*   \return  0 = Success; -1 = unknown board type
*/
int GetMoreBoardInfo(int handle, CAEN_DGTZ_BoardInfo_t BoardInfo, WaveDumpConfig_t *WDcfg)
{
    int ret;
    switch(BoardInfo.FamilyCode) {
        CAEN_DGTZ_DRS4Frequency_t freq;

    case CAEN_DGTZ_XX724_FAMILY_CODE:
    case CAEN_DGTZ_XX781_FAMILY_CODE:
    case CAEN_DGTZ_XX782_FAMILY_CODE:
    case CAEN_DGTZ_XX780_FAMILY_CODE:
        WDcfg->Nbit = 14; WDcfg->Ts = 10.0; break;
    case CAEN_DGTZ_XX720_FAMILY_CODE: WDcfg->Nbit = 12; WDcfg->Ts = 4.0;  break;
    case CAEN_DGTZ_XX721_FAMILY_CODE: WDcfg->Nbit =  8; WDcfg->Ts = 2.0;  break;
    case CAEN_DGTZ_XX731_FAMILY_CODE: WDcfg->Nbit =  8; WDcfg->Ts = 2.0;  break;
    case CAEN_DGTZ_XX751_FAMILY_CODE: WDcfg->Nbit = 10; WDcfg->Ts = 1.0;  break;
    case CAEN_DGTZ_XX761_FAMILY_CODE: WDcfg->Nbit = 10; WDcfg->Ts = 0.25;  break;
    case CAEN_DGTZ_XX740_FAMILY_CODE: WDcfg->Nbit = 12; WDcfg->Ts = 16.0; break;
    case CAEN_DGTZ_XX725_FAMILY_CODE: WDcfg->Nbit = 14; WDcfg->Ts = 4.0; break;
    case CAEN_DGTZ_XX730_FAMILY_CODE: WDcfg->Nbit = 14; WDcfg->Ts = 2.0; break;
    case CAEN_DGTZ_XX742_FAMILY_CODE: 
        WDcfg->Nbit = 12; 
        if ((ret = CAEN_DGTZ_GetDRS4SamplingFrequency(handle, &freq)) != CAEN_DGTZ_Success) return CAEN_DGTZ_CommError;
        switch (freq) {
        case CAEN_DGTZ_DRS4_1GHz:
            WDcfg->Ts = 1.0;
            break;
        case CAEN_DGTZ_DRS4_2_5GHz:
            WDcfg->Ts = (float)0.4;
            break;
        case CAEN_DGTZ_DRS4_5GHz:
            WDcfg->Ts = (float)0.2;
            break;
		case CAEN_DGTZ_DRS4_750MHz:
            WDcfg->Ts = (float)(1.0 / 750.0 * 1000.0);
            break;
        }
        switch(BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
            WDcfg->MaxGroupNumber = 4;
            break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
        default:
            WDcfg->MaxGroupNumber = 2;
            break;
        }
        break;
    default: return -1;
    }
    if (((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) ||
        (BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE) ) && WDcfg->DesMode)
        WDcfg->Ts /= 2;

    switch(BoardInfo.FamilyCode) {
    case CAEN_DGTZ_XX724_FAMILY_CODE:
    case CAEN_DGTZ_XX781_FAMILY_CODE:
    case CAEN_DGTZ_XX782_FAMILY_CODE:
    case CAEN_DGTZ_XX780_FAMILY_CODE:
    case CAEN_DGTZ_XX720_FAMILY_CODE:
    case CAEN_DGTZ_XX721_FAMILY_CODE:
    case CAEN_DGTZ_XX751_FAMILY_CODE:
    case CAEN_DGTZ_XX761_FAMILY_CODE:
    case CAEN_DGTZ_XX731_FAMILY_CODE:
        switch(BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
            WDcfg->Nch = 8;
            break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
            WDcfg->Nch = 4;
            break;
        }
        break;
    case CAEN_DGTZ_XX725_FAMILY_CODE:
    case CAEN_DGTZ_XX730_FAMILY_CODE:
        switch(BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
            WDcfg->Nch = 16;
            break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
            WDcfg->Nch = 8;
            break;
        }
        break;
    case CAEN_DGTZ_XX740_FAMILY_CODE:
        switch( BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
            WDcfg->Nch = 64;
            break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
            WDcfg->Nch = 32;
            break;
        }
        break;
    case CAEN_DGTZ_XX742_FAMILY_CODE:
        switch( BoardInfo.FormFactor) {
        case CAEN_DGTZ_VME64_FORM_FACTOR:
        case CAEN_DGTZ_VME64X_FORM_FACTOR:
            WDcfg->Nch = 36;
            break;
        case CAEN_DGTZ_DESKTOP_FORM_FACTOR:
        case CAEN_DGTZ_NIM_FORM_FACTOR:
            WDcfg->Nch = 16;
            break;
        }
        break;
    default:
        return -1;
    }
    return 0;
}

/*! \fn      int WriteRegisterBitmask(int32_t handle, uint32_t address, uint32_t data, uint32_t mask)
*   \brief   writes 'data' on register at 'address' using 'mask' as bitmask
*
*   \param   handle :   Digitizer handle
*   \param   address:   Address of the Register to write
*   \param   data   :   Data to Write on the Register
*   \param   mask   :   Bitmask to use for data masking
*   \return  0 = Success; negative numbers are error codes
*/
int WriteRegisterBitmask(int32_t handle, uint32_t address, uint32_t data, uint32_t mask) {
    int32_t ret = CAEN_DGTZ_Success;
    uint32_t d32 = 0xFFFFFFFF;

    ret = CAEN_DGTZ_ReadRegister(handle, address, &d32);
    if(ret != CAEN_DGTZ_Success)
        return ret;

    data &= mask;
    d32 &= ~mask;
    d32 |= data;
    ret = CAEN_DGTZ_WriteRegister(handle, address, d32);
    return ret;
}

static int CheckBoardFailureStatus(int handle) {

	int ret = 0;
	uint32_t status = 0;
	ret = CAEN_DGTZ_ReadRegister(handle, 0x8104, &status);
	if (ret != 0) {
		printf("Error: Unable to read board failure status.\n");
		return -1;
	}

	Sleep(200);

	//read twice (first read clears the previous status)
	ret = CAEN_DGTZ_ReadRegister(handle, 0x8104, &status);
	if (ret != 0) {
		printf("Error: Unable to read board failure status.\n");
		return -1;
	}

	if(!(status & (1 << 7))) {
		printf("Board error detected: PLL not locked.\n");
		return -1;
	}

	return 0;
}


/*! \fn      int DoProgramDigitizer(int handle, WaveDumpConfig_t WDcfg)
*   \brief   configure the digitizer according to the parameters read from
*            the cofiguration file and saved in the WDcfg data structure
*
*   \param   handle   Digitizer handle
*   \param   WDcfg:   WaveDumpConfig data structure
*   \return  0 = Success; negative numbers are error codes
*/
int DoProgramDigitizer(int handle, WaveDumpConfig_t* WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo)
{
    int i, j, ret = 0;

    /* reset the digitizer */
    ret |= CAEN_DGTZ_Reset(handle);
    if (ret != 0) {
        printf("Error: Unable to reset digitizer.\nPlease reset digitizer manually then restart the program\n");
        return -1;
    }

    // Set the waveform test bit for debugging
    if (WDcfg->TestPattern)
        ret |= CAEN_DGTZ_WriteRegister(handle, CAEN_DGTZ_BROAD_CH_CONFIGBIT_SET_ADD, 1<<3);
    // custom setting for X742 boards
    if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
        ret |= CAEN_DGTZ_SetFastTriggerDigitizing(handle,WDcfg->FastTriggerEnabled);
        ret |= CAEN_DGTZ_SetFastTriggerMode(handle,WDcfg->FastTriggerMode);
    }
    if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE)) {
        ret |= CAEN_DGTZ_SetDESMode(handle, WDcfg->DesMode);
    }
    ret |= CAEN_DGTZ_SetRecordLength(handle, WDcfg->RecordLength);
    ret |= CAEN_DGTZ_GetRecordLength(handle, &(WDcfg->RecordLength));

    if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE || BoardInfo.FamilyCode == CAEN_DGTZ_XX724_FAMILY_CODE) {
        ret |= CAEN_DGTZ_SetDecimationFactor(handle, WDcfg->DecimationFactor);
    }

    ret |= CAEN_DGTZ_SetPostTriggerSize(handle, WDcfg->PostTrigger);
    if(BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
        uint32_t pt;
        ret |= CAEN_DGTZ_GetPostTriggerSize(handle, &pt);
        WDcfg->PostTrigger = pt;
    }
    ret |= CAEN_DGTZ_SetIOLevel(handle, WDcfg->FPIOtype);
    if( WDcfg->InterruptNumEvents > 0) {
        // Interrupt handling
        if( ret |= CAEN_DGTZ_SetInterruptConfig( handle, CAEN_DGTZ_ENABLE,
            VME_INTERRUPT_LEVEL, VME_INTERRUPT_STATUS_ID,
            (uint16_t)WDcfg->InterruptNumEvents, INTERRUPT_MODE)!= CAEN_DGTZ_Success) {
                printf( "\nError configuring interrupts. Interrupts disabled\n\n");
                WDcfg->InterruptNumEvents = 0;
        }
    }
	
    ret |= CAEN_DGTZ_SetMaxNumEventsBLT(handle, WDcfg->NumEvents);
    ret |= CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
    ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, WDcfg->ExtTriggerMode);

    if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE)){
        ret |= CAEN_DGTZ_SetGroupEnableMask(handle, WDcfg->EnableMask);
        for(i=0; i<(WDcfg->Nch/8); i++) {
            if (WDcfg->EnableMask & (1<<i)) {
                if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
                    for(j=0; j<8; j++) {
                        if (WDcfg->DCoffsetGrpCh[i][j] != -1)
                            ret |= CAEN_DGTZ_SetChannelDCOffset(handle,(i*8)+j, WDcfg->DCoffsetGrpCh[i][j]);
						else
                            ret |= CAEN_DGTZ_SetChannelDCOffset(handle, (i * 8) + j, WDcfg->DCoffset[i]);

                    }
                }
                else {
                    if(WDcfg->Version_used[i] == 1)
						ret |= Set_calibrated_DCO(handle, i, WDcfg, BoardInfo);
					else
						ret |= CAEN_DGTZ_SetGroupDCOffset(handle, i, WDcfg->DCoffset[i]);
                    ret |= CAEN_DGTZ_SetGroupSelfTrigger(handle, WDcfg->ChannelTriggerMode[i], (1<<i));
                    ret |= CAEN_DGTZ_SetGroupTriggerThreshold(handle, i, WDcfg->Threshold[i]);
                    ret |= CAEN_DGTZ_SetChannelGroupMask(handle, i, WDcfg->GroupTrgEnableMask[i]);
                } 
                ret |= CAEN_DGTZ_SetTriggerPolarity(handle, i, WDcfg->PulsePolarity[i]); //.TriggerEdge

            }
        }
    } else {
        ret |= CAEN_DGTZ_SetChannelEnableMask(handle, WDcfg->EnableMask);
        for (i = 0; i < WDcfg->Nch; i++) {
            if (WDcfg->EnableMask & (1<<i)) {
				if (WDcfg->Version_used[i] == 1)
					ret |= Set_calibrated_DCO(handle, i, WDcfg, BoardInfo);
				else
					ret |= CAEN_DGTZ_SetChannelDCOffset(handle, i, WDcfg->DCoffset[i]);
                if (BoardInfo.FamilyCode != CAEN_DGTZ_XX730_FAMILY_CODE &&
                    BoardInfo.FamilyCode != CAEN_DGTZ_XX725_FAMILY_CODE)
                    ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, WDcfg->ChannelTriggerMode[i], (1<<i));
                ret |= CAEN_DGTZ_SetChannelTriggerThreshold(handle, i, WDcfg->Threshold[i]);
                ret |= CAEN_DGTZ_SetTriggerPolarity(handle, i, WDcfg->PulsePolarity[i]); //.TriggerEdge
            }
        }
        if (BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
            BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE) {
            // channel pair settings for x730 boards
            for (i = 0; i < WDcfg->Nch; i += 2) {
                if (WDcfg->EnableMask & (0x3 << i)) {
                    CAEN_DGTZ_TriggerMode_t mode = WDcfg->ChannelTriggerMode[i];
                    uint32_t pair_chmask = 0;

                    // Build mode and relevant channelmask. The behaviour is that,
                    // if the triggermode of one channel of the pair is DISABLED,
                    // this channel doesn't take part to the trigger generation.
                    // Otherwise, if both are different from DISABLED, the one of
                    // the even channel is used.
                    if (WDcfg->ChannelTriggerMode[i] != CAEN_DGTZ_TRGMODE_DISABLED) {
                        if (WDcfg->ChannelTriggerMode[i + 1] == CAEN_DGTZ_TRGMODE_DISABLED)
                            pair_chmask = (0x1 << i);
                        else
                            pair_chmask = (0x3 << i);
                    }
                    else {
                        mode = WDcfg->ChannelTriggerMode[i + 1];
                        pair_chmask = (0x2 << i);
                    }

                    pair_chmask &= WDcfg->EnableMask;
                    ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, mode, pair_chmask);
                }
            }
        }
    }
    if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {
        for(i=0; i<(WDcfg->Nch/8); i++) {
            ret |= CAEN_DGTZ_SetDRS4SamplingFrequency(handle, WDcfg->DRS4Frequency);
            ret |= CAEN_DGTZ_SetGroupFastTriggerDCOffset(handle,i,WDcfg->FTDCoffset[i]);
            ret |= CAEN_DGTZ_SetGroupFastTriggerThreshold(handle,i,WDcfg->FTThreshold[i]);
        }
    }

    /* execute generic write commands */
    for(i=0; i<WDcfg->GWn; i++)
        ret |= WriteRegisterBitmask(handle, WDcfg->GWaddr[i], WDcfg->GWdata[i], WDcfg->GWmask[i]);

    if (ret)
        printf("Warning: errors found during the programming of the digitizer.\nSome settings may not be executed\n");

    return ret;
}

/*! \fn      int ProgramDigitizerWithRelativeThreshold(int handle, WaveDumpConfig_t WDcfg)
*   \brief   configure the digitizer according to the parameters read from the cofiguration
*            file and saved in the WDcfg data structure, performing a calibration of the
*            DCOffset to set the required BASELINE_LEVEL.
*
*   \param   handle   Digitizer handle
*   \param   WDcfg:   WaveDumpConfig data structure
*   \return  0 = Success; negative numbers are error codes
*/
int ProgramDigitizerWithRelativeThreshold(int handle, WaveDumpConfig_t* WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo) {
    int32_t ret;
    if ((ret = DoProgramDigitizer(handle, WDcfg, BoardInfo)) != 0)
        return ret;
    if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) { //XX742 not considered
        if ((ret = Set_relative_Threshold(handle, WDcfg, BoardInfo)) != CAEN_DGTZ_Success) {
            fprintf(stderr, "Error setting relative threshold. Fallback to normal ProgramDigitizer.");
            DoProgramDigitizer(handle, WDcfg, BoardInfo); // Rollback
            return ret;
        }
    }
    return CAEN_DGTZ_Success;
}

/*! \fn      int ProgramDigitizerWithRelativeThreshold(int handle, WaveDumpConfig_t WDcfg)
*   \brief   configure the digitizer according to the parameters read from the cofiguration
*            file and saved in the WDcfg data structure, performing a calibration of the
*            DCOffset to set the required BASELINE_LEVEL (if provided).
*
*   \param   handle   Digitizer handle
*   \param   WDcfg:   WaveDumpConfig data structure
*   \return  0 = Success; negative numbers are error codes
*/
int ProgramDigitizer(int handle, WaveDumpConfig_t WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo) {
    int32_t ch;
    for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++) {
        if (WDcfg.EnableMask & (1 << ch) && WDcfg.Version_used[ch] == 1)
            return ProgramDigitizerWithRelativeThreshold(handle, &WDcfg, BoardInfo);
    }
    return DoProgramDigitizer(handle, &WDcfg, BoardInfo);
}

/*! \fn      void GoToNextEnabledGroup(WaveDumpRun_t *WDrun, WaveDumpConfig_t *WDcfg)
*   \brief   selects the next enabled group for plotting
*
*   \param   WDrun:   Pointer to the WaveDumpRun_t data structure
*   \param   WDcfg:   Pointer to the WaveDumpConfig_t data structure
*/
/*void GoToNextEnabledGroup(WaveDumpRun_t *WDrun, WaveDumpConfig_t *WDcfg) {
    if ((WDcfg->EnableMask) && (WDcfg->Nch>8)) {
        int orgPlotIndex = WDrun->GroupPlotIndex;
        do {
            WDrun->GroupPlotIndex = (++WDrun->GroupPlotIndex)%(WDcfg->Nch/8);
        } while( !((1 << WDrun->GroupPlotIndex)& WDcfg->EnableMask));
        if( WDrun->GroupPlotIndex != orgPlotIndex) {
            printf("Plot group set to %d\n", WDrun->GroupPlotIndex);
        }
    }
    ClearPlot();
}*/

/*! \brief   return TRUE if board descriped by 'BoardInfo' supports
*            calibration or not.
*
*   \param   BoardInfo board descriptor
*/
int32_t BoardSupportsCalibration(CAEN_DGTZ_BoardInfo_t BoardInfo) {
    return
		BoardInfo.FamilyCode == CAEN_DGTZ_XX761_FAMILY_CODE ||
        BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE ||
        BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
        BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE;
}

/*! \brief   return TRUE if board descriped by 'BoardInfo' supports
*            temperature read or not.
*
*   \param   BoardInfo board descriptor
*/
int32_t BoardSupportsTemperatureRead(CAEN_DGTZ_BoardInfo_t BoardInfo) {
    return
        BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE ||
        BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE ||
        BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE;
}

/*! \brief   Write the event data on x742 boards into the output files
*
*   \param   WDrun Pointer to the WaveDumpRun data structure
*   \param   WDcfg Pointer to the WaveDumpConfig data structure
*   \param   EventInfo Pointer to the EventInfo data structure
*   \param   Event Pointer to the Event to write
*/
void calibrate(int handle, WaveDumpRun_t *WDrun, CAEN_DGTZ_BoardInfo_t BoardInfo) {
    printf("\n");
    if (BoardSupportsCalibration(BoardInfo)) {
        if (WDrun->AcqRun == 0) {
            int32_t ret = CAEN_DGTZ_Calibrate(handle);
            if (ret == CAEN_DGTZ_Success) {
                printf("ADC Calibration check: the board is calibrated.\n");
            }
            else {
                printf("ADC Calibration failed. CAENDigitizer ERR %d\n", ret);
            }
            printf("\n");
        }
        else {
            printf("Can't run ADC calibration while acquisition is running.\n");
        }
    }
    else {
        printf("ADC Calibration not needed for this board family.\n");
    }
}


/*! \fn      void Calibrate_XX740_DC_Offset(int handle, WaveDumpConfig_t WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo)
*   \brief   calibrates DAC of enabled channel groups (only if BASELINE_SHIFT is in use)
*
*   \param   handle   Digitizer handle
*   \param   WDcfg:   Pointer to the WaveDumpConfig_t data structure
*   \param   BoardInfo: structure with the board info
*/
void Calibrate_XX740_DC_Offset(int handle, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo){
	float cal[MAX_CH];
	float offset[MAX_CH] = { 0 };
	int i = 0, acq = 0, k = 0, p=0, g = 0;
	for (i = 0; i < MAX_CH; i++)
		cal[i] = 1;
	CAEN_DGTZ_ErrorCode ret;
	CAEN_DGTZ_AcqMode_t mem_mode;
	uint32_t  AllocatedSize;

	ERROR_CODES ErrCode = ERR_NONE;
	uint32_t BufferSize;
	CAEN_DGTZ_EventInfo_t       EventInfo;
	char *buffer = NULL;
	char *EventPtr = NULL;
	CAEN_DGTZ_UINT16_EVENT_t    *Event16 = NULL;

	float avg_value[NPOINTS][MAX_CH] = { 0 };
	uint32_t dc[NPOINTS] = { 25,75 }; //test values (%)
	uint32_t groupmask = 0;

	ret = CAEN_DGTZ_GetAcquisitionMode(handle, &mem_mode);//chosen value stored
	if (ret)
		printf("Error trying to read acq mode!!\n");
	ret = CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
	if (ret)
		printf("Error trying to set acq mode!!\n");
	ret = CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_DISABLED);
	if (ret)
		printf("Error trying to set ext trigger!!\n");
	ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle, 1);
	if (ret)
		printf("Warning: error setting max BLT number\n");
	ret = CAEN_DGTZ_SetDecimationFactor(handle, 1);
	if (ret)
		printf("Error trying to set decimation factor!!\n");
	for (g = 0; g< (int32_t)BoardInfo.Channels; g++) //BoardInfo.Channels is number of groups for x740 boards
		groupmask |= (1 << g);
	ret = CAEN_DGTZ_SetGroupSelfTrigger(handle, CAEN_DGTZ_TRGMODE_DISABLED, groupmask);
	if (ret)
		printf("Error disabling self trigger\n");
	ret = CAEN_DGTZ_SetGroupEnableMask(handle, groupmask);
	if (ret)
		printf("Error enabling channel groups.\n");
	///malloc
	ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &AllocatedSize);
	if (ret) {
		ErrCode = ERR_MALLOC;
		goto QuitProgram;
	}

	ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event16);
	if (ret != CAEN_DGTZ_Success) {
		ErrCode = ERR_MALLOC;
		goto QuitProgram;
	}

	printf("Starting DAC calibration...\n");

	for (p = 0; p < NPOINTS; p++) {
		for (i = 0; i < (int32_t)BoardInfo.Channels; i++) { //BoardInfo.Channels is number of groups for x740 boards
				ret = CAEN_DGTZ_SetGroupDCOffset(handle, (uint32_t)i, (uint32_t)((float)(abs(dc[p] - 100))*(655.35)));
				if (ret)
					printf("Error setting group %d test offset\n", i);
		}
        
        Sleep(200);

	    CAEN_DGTZ_ClearData(handle);

	    ret = CAEN_DGTZ_SWStartAcquisition(handle);
	    if (ret) {
		    printf("Error starting X740 acquisition\n");
		    goto QuitProgram;
	    }

	    int value[NACQS][MAX_CH] = { 0 }; //baseline values of the NACQS
	    for (acq = 0; acq < NACQS; acq++) {
		    CAEN_DGTZ_SendSWtrigger(handle);

		    ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
		    if (ret) {
			    ErrCode = ERR_READOUT;
			    goto QuitProgram;
		    }
		    if (BufferSize == 0)
			    continue;
		    ret = CAEN_DGTZ_GetEventInfo(handle, buffer, BufferSize, 0, &EventInfo, &EventPtr);
		    if (ret) {
			    ErrCode = ERR_EVENT_BUILD;
			    goto QuitProgram;
		    }
		    // decode the event //
		    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event16);
		    if (ret) {
			    ErrCode = ERR_EVENT_BUILD;
			    goto QuitProgram;
		    }
		    for (g = 0; g < (int32_t)BoardInfo.Channels; g++) {
				    for (k = 1; k < 21; k++) //mean over 20 samples
					    value[acq][g] += (int)(Event16->DataChannel[g * 8][k]);

				    value[acq][g] = (value[acq][g] / 20);
		    }

	    }//for acq

	    ///check for clean baselines
	    for (g = 0; g < (int32_t)BoardInfo.Channels; g++) {
			    int max = 0;
			    int mpp = 0;
			    int size = (int)pow(2, (double)BoardInfo.ADC_NBits);
			    int *freq = calloc(size, sizeof(int));
			    //find the most probable value mpp
			    for (k = 0; k < NACQS; k++) {
				    if (value[k][g] > 0 && value[k][g] < size) {
					    freq[value[k][g]]++;
					    if (freq[value[k][g]] > max) {
						    max = freq[value[k][g]];
						    mpp = value[k][g];
					    }
				    }
			    }
			    free(freq);
			    //discard values too far from mpp
			    int ok = 0;
			    for (k = 0; k < NACQS; k++) {
				    if (value[k][g] >= (mpp - 5) && value[k][g] <= (mpp + 5)) {
					    avg_value[p][g] = avg_value[p][g] + (float)value[k][g];
					    ok++;
				    }
			    }
			    avg_value[p][g] = (avg_value[p][g] / (float)ok) * 100.f / (float)size;
	    }

	    CAEN_DGTZ_SWStopAcquisition(handle);
	}  //close for p

	for (g = 0; g < (int32_t)BoardInfo.Channels; g++) {
			cal[g] = ((float)(avg_value[1][g] - avg_value[0][g]) / (float)(dc[1] - dc[0]));
			offset[g] = (float)(dc[1] * avg_value[0][g] - dc[0] * avg_value[1][g]) / (float)(dc[1] - dc[0]);
			printf("Group %d DAC calibration ready.\n",g);
			printf("Cal %f   offset %f\n", cal[g], offset[g]);

			WDcfg->DAC_Calib.cal[g] = cal[g];
			WDcfg->DAC_Calib.offset[g] = offset[g];
	}

	CAEN_DGTZ_ClearData(handle);

	///free events e buffer
	CAEN_DGTZ_FreeReadoutBuffer(&buffer);

	CAEN_DGTZ_FreeEvent(handle, (void**)&Event16);

	ret |= CAEN_DGTZ_SetMaxNumEventsBLT(handle, WDcfg->NumEvents);
	ret |= CAEN_DGTZ_SetDecimationFactor(handle,WDcfg->DecimationFactor);
	ret |= CAEN_DGTZ_SetPostTriggerSize(handle, WDcfg->PostTrigger);
	ret |= CAEN_DGTZ_SetAcquisitionMode(handle, mem_mode);
	ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, WDcfg->ExtTriggerMode);
	ret |= CAEN_DGTZ_SetGroupEnableMask(handle, WDcfg->EnableMask);
	for (i = 0; i < (int)BoardInfo.Channels; i++) {
		if (WDcfg->EnableMask & (1 << i))
			ret |= CAEN_DGTZ_SetGroupSelfTrigger(handle, WDcfg->ChannelTriggerMode[i], (1 << i));
	}
	if (ret)
		printf("Error setting recorded parameters\n");

	Save_DAC_Calibration_To_Flash(handle, *WDcfg, BoardInfo);

QuitProgram:
		if (ErrCode) {
			printf("\a%s\n", ErrMsg[ErrCode]);
#ifdef WIN32
			printf("Press a key to quit\n");
			getch();
#endif
		}
}

/*! \fn      int32_t Get_current_baseline(int handle, WaveDumpConfig_t* WDcfg, char* buffer, char* EventPtr, CAEN_DGTZ_BoardInfo_t BoardInfo, int32_t *baselines)
*   \brief   gets the current baseline for every enabled channel
*
*   \param   handle     Digitizer handle
*   \param   WDcfg:     Pointer to the WaveDumpConfig_t data structure
*   \param   buffer:    Pointer to readout buffer (must already be allocated)
*   \param   EventPtr:  Pointer to an already allocated event buffer
*   \param   BoardInfo: Structure with the board info
*   \param   baselines: Array of at least 'BoardInfo.Channels' elements which will be filled with the resulting baselines
*/
int32_t Get_current_baseline(int handle, WaveDumpConfig_t* WDcfg, char* buffer, char* EventPtr, CAEN_DGTZ_BoardInfo_t BoardInfo, double *baselines) {
    CAEN_DGTZ_ErrorCode ret;
    uint32_t BufferSize = 0;
    int32_t i = 0, ch;
    uint32_t max_sample = 0x1 << WDcfg->Nbit;

    CAEN_DGTZ_UINT16_EVENT_t* Event16 = (CAEN_DGTZ_UINT16_EVENT_t*)EventPtr;
    CAEN_DGTZ_UINT8_EVENT_t* Event8 = (CAEN_DGTZ_UINT8_EVENT_t*)EventPtr;

    int32_t* histo = (int32_t*)malloc(max_sample * sizeof(*histo));
    if (histo == NULL) {
        fprintf(stderr, "Can't allocate histogram.\n");
        return ERR_MALLOC;
    }

    if ((ret = CAEN_DGTZ_ClearData(handle)) != CAEN_DGTZ_Success) {
        fprintf(stderr, "Can't clear data.\n");
        goto QuitFunction;
    }

    if ((ret = CAEN_DGTZ_SWStartAcquisition(handle)) != CAEN_DGTZ_Success) {
        fprintf(stderr, "Can't start acquisition.\n");
        goto QuitFunction;
    }

    for (i = 0; i < 100 && BufferSize == 0; i++) {
        if ((ret = CAEN_DGTZ_SendSWtrigger(handle)) != CAEN_DGTZ_Success) {
            fprintf(stderr, "Can't send SW trigger.\n");
            goto QuitFunction;
        }
        if ((ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize)) != CAEN_DGTZ_Success) {
            fprintf(stderr, "Can't read data.\n");
            goto QuitFunction;
        }
    }

    if ((ret = CAEN_DGTZ_SWStopAcquisition(handle)) != CAEN_DGTZ_Success) {
        fprintf(stderr, "Can't stop acquisition.\n");
        goto QuitFunction;
    }

    if (BufferSize == 0) {
        fprintf(stderr, "Can't get SW trigger events.\n");
        goto QuitFunction;
    }

    if ((ret = CAEN_DGTZ_DecodeEvent(handle, buffer, (void**)&EventPtr)) != CAEN_DGTZ_Success) {
        fprintf(stderr, "Can't decode events\n");
        goto QuitFunction;
    }

    memset(baselines, 0, BoardInfo.Channels * sizeof(*baselines));
    for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++) {
        if (WDcfg->EnableMask & (1 << ch)) {
            int32_t event_ch = (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) ? (ch * 8) : ch; //for x740 boards shift to channel 0 of next group
            uint32_t size = (WDcfg->Nbit == 8) ? Event8->ChSize[event_ch] : Event16->ChSize[event_ch];
            uint32_t s;
            uint32_t maxs = 0;

            memset(histo, 0, max_sample * sizeof(*histo));
            for (s = 0; s < size; s++) {
                uint16_t value = (WDcfg->Nbit == 8) ? Event8->DataChannel[event_ch][s] : Event16->DataChannel[event_ch][i];
                if (value < max_sample) {
                    histo[value]++;
                    if (histo[value] > histo[maxs])
                        maxs = value;
                }
            }

            if (WDcfg->PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityPositive)
                baselines[ch] = maxs * 100.0 / max_sample;
            else if (WDcfg->PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityNegative)
                baselines[ch] = 100.0 * (1.0 - (double)maxs / max_sample);
        }
    }

QuitFunction:
    if (histo != NULL)
        free(histo);
    return ret;
}

/*! \fn      int32_t Set_relative_Threshold(int handle, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo)
*   \brief   Finds the correct DCOffset for the BASELINE_LEVEL given in configuration file for each channel and
*            sets the threshold relative to it. To find the baseline, for each channel, two DCOffset values are
*            tried, and in the end the value given by the line passing from them is used. This function ignores
*            the DCOffset calibration previously loaded.
*
*   \param   handle   Digitizer handle
*   \param   WDcfg:   Pointer to the WaveDumpConfig_t data structure
*   \param   BoardInfo: structure with the board info
*/
int32_t Set_relative_Threshold(int handle, WaveDumpConfig_t* WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo) {
    int32_t ch, ret;
    uint32_t  AllocatedSize;
    
    char* buffer = NULL;
    char* evtbuff = NULL;
    uint32_t exdco[MAX_SET];
    double baselines[MAX_SET];
    double dcocalib[MAX_SET][2];

    //preliminary check: if baseline shift is not enabled for any channel quit
    int should_start = 0;
    for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++) {
        if (WDcfg->EnableMask & (1 << ch) && WDcfg->Version_used[ch] == 1) {
            should_start = 1;
            break;
        }
    }
    if (!should_start)
        return CAEN_DGTZ_Success;

    // Memory allocation
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &AllocatedSize);
    if (ret) {
        ret = ERR_MALLOC;
        goto QuitFunction;
    }
    ret |= CAEN_DGTZ_AllocateEvent(handle, &evtbuff);
    if (ret != CAEN_DGTZ_Success) {
        ret = ERR_MALLOC;
        goto QuitFunction;
    }
    
    for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++) {
        if (WDcfg->EnableMask & (1 << ch) && WDcfg->Version_used[ch] == 1) {
            // Assume the uncalibrated DCO is not far from the correct one
            if (WDcfg->PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityPositive)
                exdco[ch] = (uint32_t)((100.0 - dc_file[ch]) * 655.35);
            else if (WDcfg->PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityNegative)
                exdco[ch] = (uint32_t)(dc_file[ch] * 655.35);
            if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE)
                ret = CAEN_DGTZ_SetGroupDCOffset(handle, ch, exdco[ch]);
            else
                ret = CAEN_DGTZ_SetChannelDCOffset(handle, ch, exdco[ch]);
            if (ret != CAEN_DGTZ_Success) {
                fprintf(stderr, "Error setting DCOffset for channel %d\n", ch);
                goto QuitFunction;
            }
        }
    }
    // Sleep some time to let the DAC move
    Sleep(200);
    if ((ret = Get_current_baseline(handle, WDcfg, buffer, evtbuff, BoardInfo, baselines)) != CAEN_DGTZ_Success)
        goto QuitFunction;
    for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++) {
        if (WDcfg->EnableMask & (1 << ch) && WDcfg->Version_used[ch] == 1) {
            double newdco = 0.0;
            // save results of this round
            dcocalib[ch][0] = baselines[ch];
            dcocalib[ch][1] = exdco[ch];
            // ... and perform a new round, using measured value and theoretical zero
            if (WDcfg->PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityPositive)
                newdco = linear_interp(0, 65535, baselines[ch], exdco[ch], dc_file[ch]);
            else if (WDcfg->PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityNegative)
                newdco = linear_interp(0, 0, baselines[ch], exdco[ch], dc_file[ch]);
            if (newdco < 0)
                exdco[ch] = 0;
            else if (newdco > 65535)
                exdco[ch] = 65535;
            else
                exdco[ch] = (uint32_t)newdco;
            if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE)
                ret = CAEN_DGTZ_SetGroupDCOffset(handle, ch, exdco[ch]);
            else
                ret = CAEN_DGTZ_SetChannelDCOffset(handle, ch, exdco[ch]);
            if (ret != CAEN_DGTZ_Success) {
                fprintf(stderr, "Error setting DCOffset for channel %d\n", ch);
                goto QuitFunction;
            }
        }
    }
    // Sleep some time to let the DAC move
    Sleep(200);
    if ((ret = Get_current_baseline(handle, WDcfg, buffer, evtbuff, BoardInfo, baselines)) != CAEN_DGTZ_Success)
        goto QuitFunction;
    for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++) {
        if (WDcfg->EnableMask & (1 << ch) && WDcfg->Version_used[ch] == 1) {
            // Now we have two real points to use for interpolation
            double newdco = linear_interp(dcocalib[ch][0], dcocalib[ch][1], baselines[ch], exdco[ch], dc_file[ch]);
            if (newdco < 0)
                exdco[ch] = 0;
            else if (newdco > 65535)
                exdco[ch] = 65535;
            else
                exdco[ch] = (uint32_t)newdco;
            if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE)
                ret = CAEN_DGTZ_SetGroupDCOffset(handle, ch, exdco[ch]);
            else
                ret = CAEN_DGTZ_SetChannelDCOffset(handle, ch, exdco[ch]);
            if (ret != CAEN_DGTZ_Success) {
                fprintf(stderr, "Error setting DCOffset for channel %d\n", ch);
                goto QuitFunction;
            }
        }
    }
    Sleep(200);
    if ((ret = Get_current_baseline(handle, WDcfg, buffer, evtbuff, BoardInfo, baselines)) != CAEN_DGTZ_Success)
        goto QuitFunction;
    for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++) {
        if (WDcfg->EnableMask & (1 << ch) && WDcfg->Version_used[ch] == 1) {
            if (fabs((baselines[ch] - dc_file[ch]) / dc_file[ch]) > 0.05)
                fprintf(stderr, "WARNING: set BASELINE_LEVEL for ch%d differs from settings for more than 5%c.\n", ch, '%');
            uint32_t thr = 0;
            if (WDcfg->PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityPositive)
                thr = (uint32_t)(baselines[ch] / 100.0 * (0x1 << WDcfg->Nbit)) + WDcfg->Threshold[ch];
            else
                thr = (uint32_t)((100 - baselines[ch]) / 100.0 * (0x1 << WDcfg->Nbit)) - WDcfg->Threshold[ch];
            if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE)
                ret = CAEN_DGTZ_SetGroupTriggerThreshold(handle, ch, thr);
            else
                ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle, ch, thr);
            if (ret != CAEN_DGTZ_Success) {
                fprintf(stderr, "Error setting DCOffset for channel %d\n", ch);
                goto QuitFunction;
            }
        }
    }
    printf("Relative threshold correctly set.\n");

QuitFunction:
    if (evtbuff != NULL)
        CAEN_DGTZ_FreeEvent(handle, &evtbuff);
    if (buffer != NULL)
        CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    
    return ret;
}

/*! \fn      void Calibrate_DC_Offset(int handle, WaveDumpConfig_t WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo)
*   \brief   calibrates DAC of enabled channels (only if BASELINE_SHIFT is in use)
*
*   \param   handle   Digitizer handle
*   \param   WDcfg:   Pointer to the WaveDumpConfig_t data structure
*   \param   BoardInfo: structure with the board info
*/
void Calibrate_DC_Offset(int handle, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo){
	float cal[MAX_CH];
	float offset[MAX_CH] = { 0 };
	int i = 0, k = 0, p = 0, acq = 0, ch = 0;
	for (i = 0; i < MAX_CH; i++)
		cal[i] = 1;
	CAEN_DGTZ_ErrorCode ret;
	CAEN_DGTZ_AcqMode_t mem_mode;
	uint32_t  AllocatedSize;

	ERROR_CODES ErrCode = ERR_NONE;
	uint32_t BufferSize;
	CAEN_DGTZ_EventInfo_t       EventInfo;
	char *buffer = NULL;
	char *EventPtr = NULL;
	CAEN_DGTZ_UINT16_EVENT_t    *Event16 = NULL;
	CAEN_DGTZ_UINT8_EVENT_t     *Event8 = NULL;

	float avg_value[NPOINTS][MAX_CH] = { 0 };
	uint32_t dc[NPOINTS] = { 25,75 }; //test values (%)
	uint32_t chmask = 0;

	ret = CAEN_DGTZ_GetAcquisitionMode(handle, &mem_mode);//chosen value stored
	if (ret)
		printf("Error trying to read acq mode!!\n");
	ret = CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
	if (ret)
		printf("Error trying to set acq mode!!\n");
	ret = CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_DISABLED);
	if (ret)
		printf("Error trying to set ext trigger!!\n");
	for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++)
		chmask |= (1 << ch);
	ret = CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_DISABLED, chmask);
	if (ret)
		printf("Warning: error disabling channels self trigger\n");
	ret = CAEN_DGTZ_SetChannelEnableMask(handle, chmask);
	if (ret)
		printf("Warning: error enabling channels.\n");
	ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle, 1);
	if (ret)
		printf("Warning: error setting max BLT number\n");
	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE || BoardInfo.FamilyCode == CAEN_DGTZ_XX724_FAMILY_CODE) {
		ret = CAEN_DGTZ_SetDecimationFactor(handle, 1);
		if (ret)
			printf("Error trying to set decimation factor!!\n");
	}

	///malloc
	ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &AllocatedSize);
	if (ret) {
		ErrCode = ERR_MALLOC;
		goto QuitProgram;
	}
	if (WDcfg->Nbit == 8)
		ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event8);
	else {
		ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event16);
	}
	if (ret != CAEN_DGTZ_Success) {
		ErrCode = ERR_MALLOC;
		goto QuitProgram;
	}

	printf("Starting DAC calibration...\n");
	
	for (p = 0; p < NPOINTS; p++){
		//set new dco  test value to all channels
		for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++) {
				ret = CAEN_DGTZ_SetChannelDCOffset(handle, (uint32_t)ch, (uint32_t)((float)(abs(dc[p] - 100))*(655.35)));
				if (ret)
					printf("Error setting ch %d test offset\n", ch);
		}
        
        Sleep(200);

		CAEN_DGTZ_ClearData(handle);

		ret = CAEN_DGTZ_SWStartAcquisition(handle);
		if (ret){
			printf("Error starting acquisition\n");
			goto QuitProgram;
		}
		
		int value[NACQS][MAX_CH] = { 0 };//baseline values of the NACQS
		for (acq = 0; acq < NACQS; acq++){
			CAEN_DGTZ_SendSWtrigger(handle);

			ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
			if (ret) {
					ErrCode = ERR_READOUT;
					goto QuitProgram;
			}
			if (BufferSize == 0)
				continue;
			ret = CAEN_DGTZ_GetEventInfo(handle, buffer, BufferSize, 0, &EventInfo, &EventPtr);
			if (ret) {
					ErrCode = ERR_EVENT_BUILD;
					goto QuitProgram;
			}
			// decode the event //
			if (WDcfg->Nbit == 8)
				ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event8);
				else
				ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event16);

			if (ret) {
				ErrCode = ERR_EVENT_BUILD;
				goto QuitProgram;
			}

			for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++){
					for (i = 1; i < 21; i++) //mean over 20 samples
					{
						if (WDcfg->Nbit == 8)
							value[acq][ch] += (int)(Event8->DataChannel[ch][i]);
						else
							value[acq][ch] += (int)(Event16->DataChannel[ch][i]);
					}
					value[acq][ch] = (value[acq][ch] / 20);
			}

		}//for acq

		///check for clean baselines
		for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++) {
				int max = 0, ok = 0;
				int mpp = 0;
				int size = (int)pow(2, (double)BoardInfo.ADC_NBits);
				int *freq = calloc(size, sizeof(int));

				//find most probable value mpp
				for (k = 0; k < NACQS; k++) {
					if (value[k][ch] > 0 && value[k][ch] < size) {
						freq[value[k][ch]]++;
						if (freq[value[k][ch]] > max) {
							max = freq[value[k][ch]];
							mpp = value[k][ch];
						}
					}
				}
				free(freq);
				//discard values too far from mpp
				for (k = 0; k < NACQS; k++) {
					if (value[k][ch] >= (mpp - 5) && value[k][ch] <= (mpp + 5)) {
						avg_value[p][ch] = avg_value[p][ch] + (float)value[k][ch];
						ok++;
					}
				}
				//calculate final best average value
				avg_value[p][ch] = (avg_value[p][ch] / (float)ok) * 100.f / (float)size;
		}

		CAEN_DGTZ_SWStopAcquisition(handle);
	}//close for p

	for (ch = 0; ch < (int32_t)BoardInfo.Channels; ch++) {
			cal[ch] = ((float)(avg_value[1][ch] - avg_value[0][ch]) / (float)(dc[1] - dc[0]));
			offset[ch] = (float)(dc[1] * avg_value[0][ch] - dc[0] * avg_value[1][ch]) / (float)(dc[1] - dc[0]);
			printf("Channel %d DAC calibration ready.\n", ch);
			//printf("Channel %d --> Cal %f   offset %f\n", ch, cal[ch], offset[ch]);

			WDcfg->DAC_Calib.cal[ch] = cal[ch];
			WDcfg->DAC_Calib.offset[ch] = offset[ch];
	}

	CAEN_DGTZ_ClearData(handle);

	 ///free events e buffer
	CAEN_DGTZ_FreeReadoutBuffer(&buffer);
	if (WDcfg->Nbit == 8)
		CAEN_DGTZ_FreeEvent(handle, (void**)&Event8);
	else
		CAEN_DGTZ_FreeEvent(handle, (void**)&Event16);

	//reset settings
	ret |= CAEN_DGTZ_SetMaxNumEventsBLT(handle, WDcfg->NumEvents);
	ret |= CAEN_DGTZ_SetPostTriggerSize(handle, WDcfg->PostTrigger);
	ret |= CAEN_DGTZ_SetAcquisitionMode(handle, mem_mode);
	ret |= CAEN_DGTZ_SetExtTriggerInputMode(handle, WDcfg->ExtTriggerMode);
	ret |= CAEN_DGTZ_SetChannelEnableMask(handle, WDcfg->EnableMask);
	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE || BoardInfo.FamilyCode == CAEN_DGTZ_XX724_FAMILY_CODE)
		ret |= CAEN_DGTZ_SetDecimationFactor(handle, WDcfg->DecimationFactor);
	if (ret)
		printf("Error resetting some parameters after DAC calibration\n");

	//reset self trigger mode settings
	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE || BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE) {
		// channel pair settings for x730 boards
		for (i = 0; i < WDcfg->Nch; i += 2) {
			if (WDcfg->EnableMask & (0x3 << i)) {
				CAEN_DGTZ_TriggerMode_t mode = WDcfg->ChannelTriggerMode[i];
				uint32_t pair_chmask = 0;

				if (WDcfg->ChannelTriggerMode[i] != CAEN_DGTZ_TRGMODE_DISABLED) {
					if (WDcfg->ChannelTriggerMode[i + 1] == CAEN_DGTZ_TRGMODE_DISABLED)
						pair_chmask = (0x1 << i);
					else
						pair_chmask = (0x3 << i);
				}
				else {
					mode = WDcfg->ChannelTriggerMode[i + 1];
					pair_chmask = (0x2 << i);
				}

				pair_chmask &= WDcfg->EnableMask;
				ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, mode, pair_chmask);
			}
		}
	}
	else {
		for (i = 0; i < WDcfg->Nch; i++) {
			if (WDcfg->EnableMask & (1 << i))
				ret |= CAEN_DGTZ_SetChannelSelfTrigger(handle, WDcfg->ChannelTriggerMode[i], (1 << i));
		}
	}
	if (ret)
		printf("Error resetting self trigger mode after DAC calibration\n");

	Save_DAC_Calibration_To_Flash(handle, *WDcfg, BoardInfo);

QuitProgram:
	if (ErrCode) {
		printf("\a%s\n", ErrMsg[ErrCode]);
#ifdef WIN32
		printf("Press a key to quit\n");
		getch();
#endif
	}

}

/*! \fn      void Set_calibrated_DCO(int handle, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo)
*   \brief   sets the calibrated DAC value using calibration data (only if BASELINE_SHIFT is in use)
*
*   \param   handle   Digitizer handle
*   \param   WDcfg:   Pointer to the WaveDumpConfig_t data structure
*   \param   BoardInfo: structure with the board info
*/
int Set_calibrated_DCO(int handle, int ch, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo) {
	int ret = CAEN_DGTZ_Success;
	if (WDcfg->Version_used[ch] == 0) //old DC_OFFSET config, skip calibration
		return ret;
	if (WDcfg->PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityPositive) {
		WDcfg->DCoffset[ch] = (uint32_t)((float)(fabs((((float)dc_file[ch] - WDcfg->DAC_Calib.offset[ch]) / WDcfg->DAC_Calib.cal[ch]) - 100.))*(655.35));
		if (WDcfg->DCoffset[ch] > 65535) WDcfg->DCoffset[ch] = 65535;
		if (WDcfg->DCoffset[ch] < 0) WDcfg->DCoffset[ch] = 0;
	}
	else if (WDcfg->PulsePolarity[ch] == CAEN_DGTZ_PulsePolarityNegative) {
		WDcfg->DCoffset[ch] = (uint32_t)((float)(fabs(((fabs(dc_file[ch] - 100.) - WDcfg->DAC_Calib.offset[ch]) / WDcfg->DAC_Calib.cal[ch]) - 100.))*(655.35));
		if (WDcfg->DCoffset[ch] < 0) WDcfg->DCoffset[ch] = 0;
		if (WDcfg->DCoffset[ch] > 65535) WDcfg->DCoffset[ch] = 65535;
	}

	if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) {
		ret = CAEN_DGTZ_SetGroupDCOffset(handle, (uint32_t)ch, WDcfg->DCoffset[ch]);
		if (ret)
			printf("Error setting group %d offset\n", ch);
	}
	else {
		ret = CAEN_DGTZ_SetChannelDCOffset(handle, (uint32_t)ch, WDcfg->DCoffset[ch]);
		if (ret)
			printf("Error setting channel %d offset\n", ch);
	}

	return ret;
}

/***************************************************************************************/
/*VARIAVEIS GLOBAIS*/
/***************************************************************************************/

WaveDumpConfig_t   WDcfg;
WaveDumpRun_t      WDrun;
CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_Success;
int  handle = -1;
ERROR_CODES ErrCode= ERR_NONE;
char *buffer = NULL;
int isVMEDevice= 0;
CAEN_DGTZ_BoardInfo_t       BoardInfo;

CAEN_DGTZ_UINT16_EVENT_t    *Event16=NULL; /* generic event struct with 16 bit data (10, 12, 14 and 16 bit digitizers */

CAEN_DGTZ_UINT8_EVENT_t     *Event8=NULL; /* generic event struct with 8 bit data (only for 8 bit digitizers) */ 
CAEN_DGTZ_X742_EVENT_t       *Event742=NULL;  /* custom event struct with 8 bit data (only for 8 bit digitizers) */
CAEN_DGTZ_DRS4Correction_t X742Tables[MAX_X742_GROUP_SIZE];

/***************************************************************************************/
/*FUNCOES PRINCIPAIS*/
/***************************************************************************************/

//int ConfigureDigitizer(int argc, char *argv[])
int ConfigureDigitizer()
{
    int i, ch, Nb=0, Ne=0;
    uint32_t AllocatedSize;
    char ConfigFileName[100];
    int MajorNumber;
    FILE *f_ini;

    #ifdef  WIN32
    sprintf(path, "%s\\WaveDump\\", getenv("USERPROFILE"));
    _mkdir(path);
#else
    sprintf(path, "");
#endif
    int ReloadCfgStatus = 0x7FFFFFFF; // Init to the bigger positive number


    /* *************************************************************************************** */
	/* Open and parse default configuration file                                                       */
	/* *************************************************************************************** */
	memset(&WDrun, 0, sizeof(WDrun));
	memset(&WDcfg, 0, sizeof(WDcfg));
    

	/*if (argc > 1)//user entered custom filename
		strcpy(ConfigFileName, argv[1]);
	else*/ 
	/*	strcpy(ConfigFileName, DEFAULT_CONFIG_FILE);

	printf("Opening Configuration File %s\n", ConfigFileName);
	f_ini = fopen(ConfigFileName, "r");
	if (f_ini == NULL) {
        printf("Erro aqui");
		ErrCode = ERR_CONF_FILE_NOT_FOUND;
		goto QuitProgram;
	}
    printf("PArse config");
	ParseConfigFile(f_ini, &WDcfg);
	fclose(f_ini);*/

    /* *************************************************************************************** */
    /* Open the digitizer and read the board information                                       */
    /* *************************************************************************************** */
    isVMEDevice = WDcfg.BaseAddress ? 1 : 0;

    ret = CAEN_DGTZ_OpenDigitizer2(WDcfg.LinkType, (WDcfg.LinkType == CAEN_DGTZ_ETH_V4718) ? WDcfg.ipAddress:(void *)&(WDcfg.LinkNum), WDcfg.ConetNode, WDcfg.BaseAddress, &handle);
    if (ret) {
        ErrCode = ERR_DGZ_OPEN;
        goto QuitProgram;
    }
    printf("get info");
    ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
    if (ret) {
        ErrCode = ERR_BOARD_INFO_READ;
        goto QuitProgram;
    }
    printf("Connected to CAEN Digitizer Model %s\n", BoardInfo.ModelName);
    printf("ROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
    printf("AMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

    // Check firmware rivision (DPP firmwares cannot be used with WaveDump */
    sscanf(BoardInfo.AMC_FirmwareRel, "%d", &MajorNumber);
    if (MajorNumber >= 128) {
        printf("This digitizer has a DPP firmware\n");
        ErrCode = ERR_INVALID_BOARD_TYPE;
        goto QuitProgram;
    }

	/* *************************************************************************************** */
	/* Check if the board needs a specific config file and parse it instead of the default one */
	/* *************************************************************************************** */

	/*if (argc <= 1){//detect if connected board needs a specific configuration file, only if the user did not specify his configuration file
		int use_specific_file = 0;
		//Check if model x742 is in use --> use its specific configuration file
		if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {

            printf("\nArquivo config\n");
		
#ifdef LINUX 
			strcpy(ConfigFileName, "/etc/wavedump/WaveDumpConfig_X742.txt");
#else
			strcpy(ConfigFileName, "WaveDumpConfig_X742.txt");			
#endif
			printf("\nWARNING: using configuration file %s specific for Board model X742.\nEdit this file if you want to modify the default settings.\n ", ConfigFileName);
			use_specific_file = 1;
		}//Check if model x740 is in use --> use its specific configuration file
		else if (BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE) {

#ifdef LINUX 
			strcpy(ConfigFileName, "/etc/wavedump/WaveDumpConfig_X740.txt");
#else
			strcpy(ConfigFileName, "WaveDumpConfig_X740.txt");
#endif		
			printf("\nWARNING: using configuration file %s specific for Board model X740.\nEdit this file if you want to modify the default settings.\n ", ConfigFileName);
			use_specific_file = 1;
		}

		if (use_specific_file) {
			memset(&WDrun, 0, sizeof(WDrun));
			memset(&WDcfg, 0, sizeof(WDcfg));

			f_ini = fopen(ConfigFileName, "r");
			if (f_ini == NULL) {
				ErrCode = ERR_CONF_FILE_NOT_FOUND;
				goto QuitProgram;
			}
			ParseConfigFile(f_ini, &WDcfg);
			fclose(f_ini);
		}
	}*/
    printf("\nArquivo config\n");
    strcpy(ConfigFileName, "WaveDumpConfig_X742.txt");	
	printf("\nWARNING: using configuration file %s specific for Board model X742.\nEdit this file if you want to modify the default settings.\n ", ConfigFileName);
    int use_specific_file = 1;
    //printf("Checkpoint 0\n");
    if (use_specific_file) {
			memset(&WDrun, 0, sizeof(WDrun));
			memset(&WDcfg, 0, sizeof(WDcfg));
			f_ini = fopen(ConfigFileName, "r");
			if (f_ini == NULL) {
				ErrCode = ERR_CONF_FILE_NOT_FOUND;
				goto QuitProgram;
			}
            printf("Checkpoint 0\n");
			ParseConfigFile(f_ini, &WDcfg);
			fclose(f_ini);

            printf("\nCheckpoint 1\n");
		}

    // Get Number of Channels, Number of bits, Number of Groups of the board */
    ret = GetMoreBoardInfo(handle, BoardInfo, &WDcfg);
    if (ret) {
        ErrCode = ERR_INVALID_BOARD_TYPE;
        goto QuitProgram;
    }

    printf("Checkpoint 2\n");

	//Check for possible board internal errors
	ret = CheckBoardFailureStatus(handle);
	if (ret) {
		ErrCode = ERR_BOARD_FAILURE;
		goto QuitProgram;
	}

	//set default DAC calibration coefficients
	for (i = 0; i < MAX_SET; i++) {
		WDcfg.DAC_Calib.cal[i] = 1;
		WDcfg.DAC_Calib.offset[i] = 0;
	}
	//load DAC calibration data (if present in flash)
	if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)//XX742 not considered
		Load_DAC_Calibration_From_Flash(handle, &WDcfg, BoardInfo);

    // HACK
    for (ch = 0; ch < (int)BoardInfo.Channels; ch++) {
        WDcfg.DAC_Calib.cal[ch] = 1.0;
        WDcfg.DAC_Calib.offset[ch] = 0.0;
    }

    // Perform calibration (if needed).
    if (WDcfg.StartupCalibration)
        calibrate(handle, &WDrun, BoardInfo);

    printf("Checkpoint 3\n");

Restart:
    // mask the channels not available for this model
    if ((BoardInfo.FamilyCode != CAEN_DGTZ_XX740_FAMILY_CODE) && (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)){
        WDcfg.EnableMask &= (1<<WDcfg.Nch)-1;
    } else {
        WDcfg.EnableMask &= (1<<(WDcfg.Nch/8))-1;
    }
    if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) && WDcfg.DesMode) {
        WDcfg.EnableMask &= 0xAA;
    }
    if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE) && WDcfg.DesMode) {
        WDcfg.EnableMask &= 0x55;
    }
    // Set plot mask
    if ((BoardInfo.FamilyCode != CAEN_DGTZ_XX740_FAMILY_CODE) && (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)){
        WDrun.ChannelPlotMask = WDcfg.EnableMask;
    } else {
        WDrun.ChannelPlotMask = (WDcfg.FastTriggerEnabled == 0) ? 0xFF: 0x1FF;
    }
	if ((BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE) || (BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE)) {
		WDrun.GroupPlotSwitch = 0;
	}
    /* *************************************************************************************** */
    /* program the digitizer                                                                   */
    /* *************************************************************************************** */
    printf("Checkpoint 4\n");
    ret = ProgramDigitizer(handle, WDcfg, BoardInfo);
    if (ret) {
        ErrCode = ERR_DGZ_PROGRAM;
        goto QuitProgram;
    }
    
    Sleep(300);

	//check for possible failures after programming the digitizer
	ret = CheckBoardFailureStatus(handle);
	if (ret) {
		ErrCode = ERR_BOARD_FAILURE;
		goto QuitProgram;
	}
    printf("Checkpoint 5\n");

    // Select the next enabled group for plotting
    if ((WDcfg.EnableMask) && (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE || BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE))
        if( ((WDcfg.EnableMask>>WDrun.GroupPlotIndex)&0x1)==0 )
            //GoToNextEnabledGroup(&WDrun, &WDcfg);

    // Read again the board infos, just in case some of them were changed by the programming
    // (like, for example, the TSample and the number of channels if DES mode is changed)
    if(ReloadCfgStatus > 0) {
        ret = CAEN_DGTZ_GetInfo(handle, &BoardInfo);
        if (ret) {
            ErrCode = ERR_BOARD_INFO_READ;
            goto QuitProgram;
        }
        ret = GetMoreBoardInfo(handle,BoardInfo, &WDcfg);
        if (ret) {
            ErrCode = ERR_INVALID_BOARD_TYPE;
            goto QuitProgram;
        }

        // Reload Correction Tables if changed
        if(BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE && (ReloadCfgStatus & (0x1 << CFGRELOAD_CORRTABLES_BIT)) ) {
            if(WDcfg.useCorrections != -1) { // Use Manual Corrections
                uint32_t GroupMask = 0;

                // Disable Automatic Corrections
                if ((ret = CAEN_DGTZ_DisableDRS4Correction(handle)) != CAEN_DGTZ_Success)
                    goto QuitProgram;

                // Load the Correction Tables from the Digitizer flash
                if ((ret = CAEN_DGTZ_GetCorrectionTables(handle, WDcfg.DRS4Frequency, (void*)X742Tables)) != CAEN_DGTZ_Success)
                    goto QuitProgram;

                if(WDcfg.UseManualTables != -1) { // The user wants to use some custom tables
                    uint32_t gr;
					int32_t clret;
					
                    GroupMask = WDcfg.UseManualTables;

                    for(gr = 0; gr < WDcfg.MaxGroupNumber; gr++) {
                        if (((GroupMask>>gr)&0x1) == 0)
                            continue;
                        if ((clret = LoadCorrectionTable(WDcfg.TablesFilenames[gr], &(X742Tables[gr]))) != 0)
                            printf("Error [%d] loading custom table from file '%s' for group [%u].\n", clret, WDcfg.TablesFilenames[gr], gr);
                    }
                }
                // Save to file the Tables read from flash
                GroupMask = (~GroupMask) & ((0x1<<WDcfg.MaxGroupNumber)-1);
                SaveCorrectionTables("X742Table", GroupMask, X742Tables);
            }
            else { // Use Automatic Corrections
                if ((ret = CAEN_DGTZ_LoadDRS4CorrectionData(handle, WDcfg.DRS4Frequency)) != CAEN_DGTZ_Success)
                    goto QuitProgram;
                if ((ret = CAEN_DGTZ_EnableDRS4Correction(handle)) != CAEN_DGTZ_Success)
                    goto QuitProgram;
            }
        }
    }

    // Allocate memory for the event data and readout buffer
    if(WDcfg.Nbit == 8)
        ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event8);
    else {
        if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
            ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event16);
        }
        else {
            ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&Event742);
        }
    }
    if (ret != CAEN_DGTZ_Success) {
        ErrCode = ERR_MALLOC;
        goto QuitProgram;
    }
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer,&AllocatedSize); /* WARNING: This malloc must be done after the digitizer programming */
    if (ret) {
        ErrCode = ERR_MALLOC;
        goto QuitProgram;
    }

	if (WDrun.Restart && WDrun.AcqRun) 
	{
        Sleep(300);
		CAEN_DGTZ_SWStartAcquisition(handle);
	}
    else
        printf("[s] start/stop the acquisition, [q] quit, [SPACE] help\n");

    return 0;

QuitProgram:
    return 1;
}


int printResults()
{

    int i, ch, Nb=0, Ne=0;
    uint32_t BufferSize, NumEvents;
    char *EventPtr = NULL;
    uint64_t CurrentTime, PrevRateTime, ElapsedTime;
    int nCycles= 0;
    CAEN_DGTZ_EventInfo_t       EventInfo;

        #ifdef  WIN32
    sprintf(path, "%s\\WaveDump\\", getenv("USERPROFILE"));
    _mkdir(path);
#else
    sprintf(path, "");
#endif


    WDrun.Restart = 0;
    PrevRateTime = get_time();
    /* *************************************************************************************** */
    /* Readout Loop                                                                            */
    /* *************************************************************************************** */
    WDrun.SingleWrite = 1;
    //printf("teste: %d", WDrun.SingleWrite);
    //printf("pressione uma tecla para salvar um evento\n");
    //getch();
    //printf("pressionado");
    CAEN_DGTZ_SWStartAcquisition(handle);
    WDrun.AcqRun = 1;
    int gr, j, Size;
    while(WDrun.SingleWrite) {	
        
        // Check for keyboard commands (key pressed)
        //CheckKeyboardCommands(handle, &WDrun, &WDcfg, BoardInfo); //não vai precisar
        

        if (WDrun.AcqRun == 0)
            continue;
        

        /* Send a software trigger */
        if (WDrun.ContinuousTrigger) {
            CAEN_DGTZ_SendSWtrigger(handle);
        }

        /* Wait for interrupt (if enabled) */
        if (WDcfg.InterruptNumEvents > 0) {
            int32_t boardId;
            int VMEHandle = -1;
            int InterruptMask = (1 << VME_INTERRUPT_LEVEL);

            BufferSize = 0;
            NumEvents = 0;
            // Interrupt handling
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIRQWait ((CAEN_DGTZ_ConnectionType)WDcfg.LinkType, WDcfg.LinkNum, WDcfg.ConetNode, (uint8_t)InterruptMask, INTERRUPT_TIMEOUT, &VMEHandle);
            }
            else
                ret = CAEN_DGTZ_IRQWait(handle, INTERRUPT_TIMEOUT);
            if (ret == CAEN_DGTZ_Timeout)  // No active interrupt requests
                goto InterruptTimeout;
            if (ret != CAEN_DGTZ_Success)  {
                ErrCode = ERR_INTERRUPT;
                goto QuitProgram;
            }
            // Interrupt Ack
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIACKCycle(VMEHandle, VME_INTERRUPT_LEVEL, &boardId);
                if ((ret != CAEN_DGTZ_Success) || (boardId != VME_INTERRUPT_STATUS_ID)) {
                    goto InterruptTimeout;
                } else {
                    if (INTERRUPT_MODE == CAEN_DGTZ_IRQ_MODE_ROAK)
                        ret = CAEN_DGTZ_RearmInterrupt(handle);
                }
            }
        }

        /* Read data from the board */
         ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
        if (ret) {

            ErrCode = ERR_READOUT;
            goto QuitProgram;
        }
        NumEvents = 0;
        if (BufferSize != 0) {
            ret = CAEN_DGTZ_GetNumEvents(handle, buffer, BufferSize, &NumEvents);
            if (ret) {
                ErrCode = ERR_READOUT;
                goto QuitProgram;
            }
        }
		else {
			uint32_t lstatus;
			ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &lstatus);
			if (ret) {
				printf("Warning: Failure reading reg:%x (%d)\n", CAEN_DGTZ_ACQ_STATUS_ADD, ret);
			}
			else {
				if (lstatus & (0x1 << 19)) {
					ErrCode = ERR_OVERTEMP;
					goto QuitProgram;
				}
			}
		}

InterruptTimeout:
        /* Calculate throughput and trigger rate (every second) */
        Nb += BufferSize;
        Ne += NumEvents;
        CurrentTime = get_time();
        ElapsedTime = CurrentTime - PrevRateTime;

        nCycles++;
        if (ElapsedTime > 1000) {
            if (Nb == 0)
                if (ret == CAEN_DGTZ_Timeout) printf ("Timeout...\n"); else printf("No data...\n");
            else
                printf("Reading at %.2f MB/s (Trg Rate: %.2f Hz)\n", (float)Nb/((float)ElapsedTime*1048.576f), (float)Ne*1000.0f/(float)ElapsedTime);
            nCycles= 0;
            Nb = 0;
            Ne = 0;
            PrevRateTime = CurrentTime;
        }

        /* Analyze data */
        for(i = 0; i < (int)NumEvents; i++) {

            /* Get one event from the readout buffer */
            ret = CAEN_DGTZ_GetEventInfo(handle, buffer, BufferSize, i, &EventInfo, &EventPtr);
            if (ret) {
                ErrCode = ERR_EVENT_BUILD;
                goto QuitProgram;
            }
            /* decode the event */
            if (WDcfg.Nbit == 8) 
                ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event8);
            else
                if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event16);
                }
                else {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event742);
                    if (WDcfg.useCorrections != -1) { // if manual corrections (não é usado)
                        uint32_t gr;
                        for (gr = 0; gr < WDcfg.MaxGroupNumber; gr++) {
                            if ( ((WDcfg.EnableMask >> gr) & 0x1) == 0)
                                continue;
                            ApplyDataCorrection( &(X742Tables[gr]), WDcfg.DRS4Frequency, WDcfg.useCorrections, &(Event742->DataGroup[gr]));
                        }
                    }
                }
                if (ret) {
                    ErrCode = ERR_EVENT_BUILD;
                    goto QuitProgram;
                }

                /* Write Event data to file */
                /*if (WDrun.ContinuousWrite || WDrun.SingleWrite) {
                    printf("pressione para salvar\n");
                    getch();
                    // Note: use a thread here to allow parallel readout and file writing
                    if (BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE) {	
                        //ret = WriteOutputFilesx742(&WDcfg, &WDrun, &EventInfo, Event742); 
                        //printf("arquivo escrito\n");
                        //getch();
                        }
                    }
                    else if (WDcfg.Nbit == 8) {
                        ret = WriteOutputFiles(&WDcfg, &WDrun, &EventInfo, Event8);
                        printf("arquivo escrito\n");
                        getch();
                    }
                    else {
                        ret = WriteOutputFiles(&WDcfg, &WDrun, &EventInfo, Event16);
                        
                    }
                    if (ret) {
                        ErrCode = ERR_OUTFILE_WRITE;
                        goto QuitProgram;
                    }*/

                    if (WDrun.SingleWrite) {
                        printf("Single Event saved to output files\n");
                        WDrun.SingleWrite = 0;
                        CAEN_DGTZ_SWStopAcquisition(handle);
                        printf("Aquisicao parada\n");
                        //*********************************************************************
                        //printa os eventos
                            ch=0;
                            gr=0;
                            //for(ch=0; ch<2; ch++) {
                            int Size = Event742->DataGroup[gr].ChSize[ch];
                            printf("Canal %d\n", ch);
                                for(j=0; j<Size; j++) {
                                printf("%f\n", Event742->DataGroup[gr].DataChannel[ch][j]);
                            }
                        //*********************************************************************
                        //getch();
                    }
        return 0;
        QuitProgram:
        return 1;
        }
    }
}


int getMinValue()
{

    int i, ch, Nb=0, Ne=0;
    uint32_t BufferSize, NumEvents;
    char *EventPtr = NULL;
    uint64_t CurrentTime, PrevRateTime, ElapsedTime;
    int nCycles= 0;
    CAEN_DGTZ_EventInfo_t       EventInfo;

        #ifdef  WIN32
    sprintf(path, "%s\\WaveDump\\", getenv("USERPROFILE"));
    _mkdir(path);
    #else
        sprintf(path, "");
    #endif


    WDrun.Restart = 0;
    PrevRateTime = get_time();
    /* *************************************************************************************** */
    /* Readout Loop                                                                            */
    /* *************************************************************************************** */
    WDrun.SingleWrite = 1;
    //printf("teste: %d", WDrun.SingleWrite);
    //printf("pressione uma tecla para salvar um evento\n");
    //getch();
    //printf("pressionado");
    CAEN_DGTZ_SWStartAcquisition(handle);
    WDrun.AcqRun = 1;
    int gr, j, Size;
    int amplitude;
    while(WDrun.SingleWrite) {	
        
        // Check for keyboard commands (key pressed)
        //CheckKeyboardCommands(handle, &WDrun, &WDcfg, BoardInfo); //não vai precisar
        

        if (WDrun.AcqRun == 0)
            continue;
        

        /* Send a software trigger */
        if (WDrun.ContinuousTrigger) {
            CAEN_DGTZ_SendSWtrigger(handle);
        }

        /* Wait for interrupt (if enabled) */
        if (WDcfg.InterruptNumEvents > 0) {
            int32_t boardId;
            int VMEHandle = -1;
            int InterruptMask = (1 << VME_INTERRUPT_LEVEL);

            BufferSize = 0;
            NumEvents = 0;
            // Interrupt handling
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIRQWait ((CAEN_DGTZ_ConnectionType)WDcfg.LinkType, WDcfg.LinkNum, WDcfg.ConetNode, (uint8_t)InterruptMask, INTERRUPT_TIMEOUT, &VMEHandle);
            }
            else
                ret = CAEN_DGTZ_IRQWait(handle, INTERRUPT_TIMEOUT);
            if (ret == CAEN_DGTZ_Timeout)  // No active interrupt requests
                goto InterruptTimeout;
            if (ret != CAEN_DGTZ_Success)  {
                ErrCode = ERR_INTERRUPT;
                goto QuitProgram;
            }
            // Interrupt Ack
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIACKCycle(VMEHandle, VME_INTERRUPT_LEVEL, &boardId);
                if ((ret != CAEN_DGTZ_Success) || (boardId != VME_INTERRUPT_STATUS_ID)) {
                    goto InterruptTimeout;
                } else {
                    if (INTERRUPT_MODE == CAEN_DGTZ_IRQ_MODE_ROAK)
                        ret = CAEN_DGTZ_RearmInterrupt(handle);
                }
            }
        }

        /* Read data from the board */
         ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
        if (ret) {

            ErrCode = ERR_READOUT;
            goto QuitProgram;
        }
        NumEvents = 0;
        if (BufferSize != 0) {
            ret = CAEN_DGTZ_GetNumEvents(handle, buffer, BufferSize, &NumEvents);
            if (ret) {
                ErrCode = ERR_READOUT;
                goto QuitProgram;
            }
        }
		else {
			uint32_t lstatus;
			ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &lstatus);
			if (ret) {
				printf("Warning: Failure reading reg:%x (%d)\n", CAEN_DGTZ_ACQ_STATUS_ADD, ret);
			}
			else {
				if (lstatus & (0x1 << 19)) {
					ErrCode = ERR_OVERTEMP;
					goto QuitProgram;
				}
			}
		}

InterruptTimeout:
        /* Calculate throughput and trigger rate (every second) */
        Nb += BufferSize;
        Ne += NumEvents;
        CurrentTime = get_time();
        ElapsedTime = CurrentTime - PrevRateTime;

        nCycles++;
        if (ElapsedTime > 1000) {
            if (Nb == 0)
                if (ret == CAEN_DGTZ_Timeout) printf ("Timeout...\n"); else printf("No data...\n");
            else
                printf("Reading at %.2f MB/s (Trg Rate: %.2f Hz)\n", (float)Nb/((float)ElapsedTime*1048.576f), (float)Ne*1000.0f/(float)ElapsedTime);
            nCycles= 0;
            Nb = 0;
            Ne = 0;
            PrevRateTime = CurrentTime;
        }

        /* Analyze data */
        for(i = 0; i < (int)NumEvents; i++) {

            /* Get one event from the readout buffer */
            ret = CAEN_DGTZ_GetEventInfo(handle, buffer, BufferSize, i, &EventInfo, &EventPtr);
            if (ret) {
                ErrCode = ERR_EVENT_BUILD;
                goto QuitProgram;
            }
            /* decode the event */
            if (WDcfg.Nbit == 8) 
                ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event8);
            else
                if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event16);
                }
                else {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event742);
                    if (WDcfg.useCorrections != -1) { // if manual corrections (não é usado)
                        uint32_t gr;
                        for (gr = 0; gr < WDcfg.MaxGroupNumber; gr++) {
                            if ( ((WDcfg.EnableMask >> gr) & 0x1) == 0)
                                continue;
                            ApplyDataCorrection( &(X742Tables[gr]), WDcfg.DRS4Frequency, WDcfg.useCorrections, &(Event742->DataGroup[gr]));
                        }
                    }
                }
                if (ret) {
                    ErrCode = ERR_EVENT_BUILD;
                    goto QuitProgram;
                }

                    if (WDrun.SingleWrite) {
                        printf("Single Event saved to output files\n");
                        WDrun.SingleWrite = 0;
                        CAEN_DGTZ_SWStopAcquisition(handle);
                        printf("Aquisicao parada\n");
                        //*********************************************************************
                        //encontra o valor da baseline
                            ch=0;
                            gr=0;
                            int Size = Event742->DataGroup[gr].ChSize[ch];
                            
                            int baseline100 = 0;
                            printf("\npara 100 ultimos valores: ");
                            for(i = Size; i>= Size - 100; i--){
                                baseline100 +=  Event742->DataGroup[gr].DataChannel[ch][i];
                            }
                                baseline100 = baseline100/100;
                            printf("%d", baseline100);

                            int minval = 0xFFFF;
                            int val = 0;
                            for(i=0; i<Size; i++){
                                val = Event742->DataGroup[gr].DataChannel[ch][i];
                                if(val<minval) minval = val;
                            }

                            printf("\no menor valor eh: %d\n", minval);

                            amplitude = abs(baseline100 - minval);
                            printf("Amplitude = %d", amplitude);
                            //printf("A amplitude eh de: %d\n", amplitude);

                        //*********************************************************************
                    }
        return amplitude;
        QuitProgram:
        return -1;
        }
    }
}

int getMaxValue()
{
    int i, ch, Nb=0, Ne=0;
    uint32_t BufferSize, NumEvents;
    char *EventPtr = NULL;
    uint64_t CurrentTime, PrevRateTime, ElapsedTime;
    int nCycles= 0;
    CAEN_DGTZ_EventInfo_t       EventInfo;

        #ifdef  WIN32
    sprintf(path, "%s\\WaveDump\\", getenv("USERPROFILE"));
    _mkdir(path);
    #else
        sprintf(path, "");
    #endif


    WDrun.Restart = 0;
    PrevRateTime = get_time();
    /* *************************************************************************************** */
    /* Readout Loop                                                                            */
    /* *************************************************************************************** */
    WDrun.SingleWrite = 1;
    //printf("teste: %d", WDrun.SingleWrite);
    //printf("pressione uma tecla para salvar um evento\n");
    //getch();
    //printf("pressionado");
    CAEN_DGTZ_SWStartAcquisition(handle);
    WDrun.AcqRun = 1;
    int gr, j, Size;
    while(WDrun.SingleWrite) {	
      
        if (WDrun.AcqRun == 0)
            continue;
        

        /* Send a software trigger */
        if (WDrun.ContinuousTrigger) {
            CAEN_DGTZ_SendSWtrigger(handle);
        }

        /* Wait for interrupt (if enabled) */
        if (WDcfg.InterruptNumEvents > 0) {
            int32_t boardId;
            int VMEHandle = -1;
            int InterruptMask = (1 << VME_INTERRUPT_LEVEL);

            BufferSize = 0;
            NumEvents = 0;
            // Interrupt handling
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIRQWait ((CAEN_DGTZ_ConnectionType)WDcfg.LinkType, WDcfg.LinkNum, WDcfg.ConetNode, (uint8_t)InterruptMask, INTERRUPT_TIMEOUT, &VMEHandle);
            }
            else
                ret = CAEN_DGTZ_IRQWait(handle, INTERRUPT_TIMEOUT);
            if (ret == CAEN_DGTZ_Timeout)  // No active interrupt requests
                goto InterruptTimeout;
            if (ret != CAEN_DGTZ_Success)  {
                ErrCode = ERR_INTERRUPT;
                goto QuitProgram;
            }
            // Interrupt Ack
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIACKCycle(VMEHandle, VME_INTERRUPT_LEVEL, &boardId);
                if ((ret != CAEN_DGTZ_Success) || (boardId != VME_INTERRUPT_STATUS_ID)) {
                    goto InterruptTimeout;
                } else {
                    if (INTERRUPT_MODE == CAEN_DGTZ_IRQ_MODE_ROAK)
                        ret = CAEN_DGTZ_RearmInterrupt(handle);
                }
            }
        }

        /* Read data from the board */
         ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
        if (ret) {

            ErrCode = ERR_READOUT;
            goto QuitProgram;
        }
        NumEvents = 0;
        if (BufferSize != 0) {
            ret = CAEN_DGTZ_GetNumEvents(handle, buffer, BufferSize, &NumEvents);
            if (ret) {
                ErrCode = ERR_READOUT;
                goto QuitProgram;
            }
        }
		else {
			uint32_t lstatus;
			ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &lstatus);
			if (ret) {
				printf("Warning: Failure reading reg:%x (%d)\n", CAEN_DGTZ_ACQ_STATUS_ADD, ret);
			}
			else {
				if (lstatus & (0x1 << 19)) {
					ErrCode = ERR_OVERTEMP;
					goto QuitProgram;
				}
			}
		}

InterruptTimeout:
        /* Calculate throughput and trigger rate (every second) */
        Nb += BufferSize;
        Ne += NumEvents;
        CurrentTime = get_time();
        ElapsedTime = CurrentTime - PrevRateTime;

        nCycles++;
        if (ElapsedTime > 1000) {
            if (Nb == 0)
                if (ret == CAEN_DGTZ_Timeout) printf ("Timeout...\n"); else printf("No data...\n");
            else
                printf("Reading at %.2f MB/s (Trg Rate: %.2f Hz)\n", (float)Nb/((float)ElapsedTime*1048.576f), (float)Ne*1000.0f/(float)ElapsedTime);
            nCycles= 0;
            Nb = 0;
            Ne = 0;
            PrevRateTime = CurrentTime;
        }

        /* Analyze data */
        for(i = 0; i < (int)NumEvents; i++) {

            /* Get one event from the readout buffer */
            ret = CAEN_DGTZ_GetEventInfo(handle, buffer, BufferSize, i, &EventInfo, &EventPtr);
            if (ret) {
                ErrCode = ERR_EVENT_BUILD;
                goto QuitProgram;
            }
            /* decode the event */
            if (WDcfg.Nbit == 8) 
                ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event8);
            else
                if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event16);
                }
                else {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event742);
                    if (WDcfg.useCorrections != -1) { // if manual corrections (não é usado)
                        uint32_t gr;
                        for (gr = 0; gr < WDcfg.MaxGroupNumber; gr++) {
                            if ( ((WDcfg.EnableMask >> gr) & 0x1) == 0)
                                continue;
                            ApplyDataCorrection( &(X742Tables[gr]), WDcfg.DRS4Frequency, WDcfg.useCorrections, &(Event742->DataGroup[gr]));
                        }
                    }
                }
                if (ret) {
                    ErrCode = ERR_EVENT_BUILD;
                    goto QuitProgram;
                }

                    if (WDrun.SingleWrite) {
                        printf("Single Event saved to output files\n");
                        WDrun.SingleWrite = 0;
                        CAEN_DGTZ_SWStopAcquisition(handle);
                        printf("Aquisicao parada\n");
                        //*********************************************************************
                        //encontra o valor da baseline
                            ch=0;
                            gr=0;
                            int Size = Event742->DataGroup[gr].ChSize[ch];
                            
                            int baseline100 = 0;
                            printf("\npara 100 ultimos valores: ");
                            for(i = Size; i>= Size - 100; i--){
                                baseline100 +=  Event742->DataGroup[gr].DataChannel[ch][i];
                            }
                                baseline100 = baseline100/100;
                            printf("%d", baseline100);

                            //encontra o valor maximo
                            int maxval = 0;
                            int val = 0;
                            for(i=0; i<Size; i++){
                                val = Event742->DataGroup[gr].DataChannel[ch][i];
                                if(val>maxval) maxval = val;
                            }

                            printf("\no menor valor eh: %d\n", maxval);

                            //Encontra a amplitude em relacao a baseline
                            int amplitude;
                            amplitude = abs(baseline100 - maxval);
                            printf("A amplitude eh de: %d\n", amplitude);
                        //*********************************************************************
                    }
        return 0;
        QuitProgram:
        return 1;
        }
    }
}

int CloseDigitizer()
{
        int i, ch, Nb=0, Ne=0;

    /* stop the acquisition */
    CAEN_DGTZ_SWStopAcquisition(handle);

    /* close the output files and free histograms*/
    for (ch = 0; ch < WDcfg.Nch; ch++) {
        if (WDrun.fout[ch])
            fclose(WDrun.fout[ch]);
        if (WDrun.Histogram[ch])
            free(WDrun.Histogram[ch]);
    }

    /* close the device and free the buffers */
    if(Event8)
        CAEN_DGTZ_FreeEvent(handle, (void**)&Event8);
    if(Event16)
        CAEN_DGTZ_FreeEvent(handle, (void**)&Event16);
    CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    CAEN_DGTZ_CloseDigitizer(handle);

    return 0;
}

int getAvgMinValue(int numSamples)
{

    int i, ch, Nb=0, Ne=0;
    int avgAmplitude = 0;
    uint32_t BufferSize, NumEvents;
    char *EventPtr = NULL;
    uint64_t CurrentTime, PrevRateTime, ElapsedTime;
    int nCycles= 0;
    CAEN_DGTZ_EventInfo_t       EventInfo;

        #ifdef  WIN32
    sprintf(path, "%s\\WaveDump\\", getenv("USERPROFILE"));
    _mkdir(path);
    #else
        sprintf(path, "");
    #endif


    WDrun.Restart = 0;
    PrevRateTime = get_time();
    /* *************************************************************************************** */
    /* Readout Loop                                                                            */
    /* *************************************************************************************** */
    WDrun.SingleWrite = 1;
    //printf("teste: %d", WDrun.SingleWrite);
    //printf("pressione uma tecla para salvar um evento\n");
    //getch();
    //printf("pressionado");
    CAEN_DGTZ_SWStartAcquisition(handle);
    WDrun.AcqRun = 1;
    int gr, j, Size;
    int amplitude;
    for(int k = 0; k<numSamples; k++) {	
        
        // Check for keyboard commands (key pressed)
        //CheckKeyboardCommands(handle, &WDrun, &WDcfg, BoardInfo); //não vai precisar
        

        if (WDrun.AcqRun == 0)
            continue;
        

        /* Send a software trigger */
        if (WDrun.ContinuousTrigger) {
            CAEN_DGTZ_SendSWtrigger(handle);
        }

        /* Wait for interrupt (if enabled) */
        if (WDcfg.InterruptNumEvents > 0) {
            int32_t boardId;
            int VMEHandle = -1;
            int InterruptMask = (1 << VME_INTERRUPT_LEVEL);

            BufferSize = 0;
            NumEvents = 0;
            // Interrupt handling
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIRQWait ((CAEN_DGTZ_ConnectionType)WDcfg.LinkType, WDcfg.LinkNum, WDcfg.ConetNode, (uint8_t)InterruptMask, INTERRUPT_TIMEOUT, &VMEHandle);
            }
            else
                ret = CAEN_DGTZ_IRQWait(handle, INTERRUPT_TIMEOUT);
            if (ret == CAEN_DGTZ_Timeout)  // No active interrupt requests
                goto InterruptTimeout;
            if (ret != CAEN_DGTZ_Success)  {
                ErrCode = ERR_INTERRUPT;
                goto QuitProgram;
            }
            // Interrupt Ack
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIACKCycle(VMEHandle, VME_INTERRUPT_LEVEL, &boardId);
                if ((ret != CAEN_DGTZ_Success) || (boardId != VME_INTERRUPT_STATUS_ID)) {
                    goto InterruptTimeout;
                } else {
                    if (INTERRUPT_MODE == CAEN_DGTZ_IRQ_MODE_ROAK)
                        ret = CAEN_DGTZ_RearmInterrupt(handle);
                }
            }
        }

        /* Read data from the board */
         ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
        if (ret) {

            ErrCode = ERR_READOUT;
            goto QuitProgram;
        }
        NumEvents = 0;
        if (BufferSize != 0) {
            ret = CAEN_DGTZ_GetNumEvents(handle, buffer, BufferSize, &NumEvents);
            if (ret) {
                ErrCode = ERR_READOUT;
                goto QuitProgram;
            }
        }
		else {
			uint32_t lstatus;
			ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &lstatus);
			if (ret) {
				printf("Warning: Failure reading reg:%x (%d)\n", CAEN_DGTZ_ACQ_STATUS_ADD, ret);
			}
			else {
				if (lstatus & (0x1 << 19)) {
					ErrCode = ERR_OVERTEMP;
					goto QuitProgram;
				}
			}
		}

InterruptTimeout:
        /* Calculate throughput and trigger rate (every second) */
        Nb += BufferSize;
        Ne += NumEvents;
        CurrentTime = get_time();
        ElapsedTime = CurrentTime - PrevRateTime;

        nCycles++;
        if (ElapsedTime > 1000) {
            if (Nb == 0)
                if (ret == CAEN_DGTZ_Timeout) printf ("Timeout...\n"); else printf("No data...\n");
            else
                printf("Reading at %.2f MB/s (Trg Rate: %.2f Hz)\n", (float)Nb/((float)ElapsedTime*1048.576f), (float)Ne*1000.0f/(float)ElapsedTime);
            nCycles= 0;
            Nb = 0;
            Ne = 0;
            PrevRateTime = CurrentTime;
        }

        /* Analyze data */
        for(i = 0; i < (int)NumEvents; i++) {

            /* Get one event from the readout buffer */
            ret = CAEN_DGTZ_GetEventInfo(handle, buffer, BufferSize, i, &EventInfo, &EventPtr);
            if (ret) {
                ErrCode = ERR_EVENT_BUILD;
                goto QuitProgram;
            }
            /* decode the event */
            if (WDcfg.Nbit == 8) 
                ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event8);
            else
                if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event16);
                }
                else {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event742);
                    if (WDcfg.useCorrections != -1) { // if manual corrections (não é usado)
                        uint32_t gr;
                        for (gr = 0; gr < WDcfg.MaxGroupNumber; gr++) {
                            if ( ((WDcfg.EnableMask >> gr) & 0x1) == 0)
                                continue;
                            ApplyDataCorrection( &(X742Tables[gr]), WDcfg.DRS4Frequency, WDcfg.useCorrections, &(Event742->DataGroup[gr]));
                        }
                    }
                }
                if (ret) {
                    ErrCode = ERR_EVENT_BUILD;
                    goto QuitProgram;
                }

                //*********************************************************************
                //encontra o valor da baseline
                ch=0;
                gr=0;
                int Size = Event742->DataGroup[gr].ChSize[ch];
                            
                int baseline100 = 0;
                //printf("\npara 100 ultimos valores: ");
                for(i = Size; i>= Size - 100; i--){
                    baseline100 +=  Event742->DataGroup[gr].DataChannel[ch][i];
                }
                baseline100 = baseline100/100;
                            //printf("%d", baseline100);

                int minval = 0xFFFF;
                int val = 0;
                for(i=0; i<Size; i++){
                    val = Event742->DataGroup[gr].DataChannel[ch][i];
                    if(val<minval) minval = val;
                }

                            //printf("\no menor valor eh: %d\n", minval);

                amplitude = abs(baseline100 - minval);
                            //printf("Amplitude = %d", amplitude);
                            //printf("A amplitude eh de: %d\n", amplitude);

                avgAmplitude += amplitude; 
                //*********************************************************************
        }
    }
    CAEN_DGTZ_SWStopAcquisition(handle);
    return avgAmplitude/numSamples;
    QuitProgram:
    return -1;
}

int getAvgMaxValue(int numSamples)
{
    int i, ch, Nb=0, Ne=0;
    int avgAmplitude = 0;
    uint32_t BufferSize, NumEvents;
    char *EventPtr = NULL;
    uint64_t CurrentTime, PrevRateTime, ElapsedTime;
    int nCycles= 0;
    CAEN_DGTZ_EventInfo_t       EventInfo;

        #ifdef  WIN32
    sprintf(path, "%s\\WaveDump\\", getenv("USERPROFILE"));
    _mkdir(path);
    #else
        sprintf(path, "");
    #endif


    WDrun.Restart = 0;
    PrevRateTime = get_time();
    /* *************************************************************************************** */
    /* Readout Loop                                                                            */
    /* *************************************************************************************** */
    WDrun.SingleWrite = 1;
    //printf("teste: %d", WDrun.SingleWrite);
    //printf("pressione uma tecla para salvar um evento\n");
    //getch();
    //printf("pressionado");
    CAEN_DGTZ_SWStartAcquisition(handle);
    WDrun.AcqRun = 1;
    int gr, j, Size;
    int amplitude;
    for(int k = 0; k<numSamples; k++) {	
        
        // Check for keyboard commands (key pressed)
        //CheckKeyboardCommands(handle, &WDrun, &WDcfg, BoardInfo); //não vai precisar
        

        if (WDrun.AcqRun == 0)
            continue;
        

        /* Send a software trigger */
        if (WDrun.ContinuousTrigger) {
            CAEN_DGTZ_SendSWtrigger(handle);
        }

        /* Wait for interrupt (if enabled) */
        if (WDcfg.InterruptNumEvents > 0) {
            int32_t boardId;
            int VMEHandle = -1;
            int InterruptMask = (1 << VME_INTERRUPT_LEVEL);

            BufferSize = 0;
            NumEvents = 0;
            // Interrupt handling
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIRQWait ((CAEN_DGTZ_ConnectionType)WDcfg.LinkType, WDcfg.LinkNum, WDcfg.ConetNode, (uint8_t)InterruptMask, INTERRUPT_TIMEOUT, &VMEHandle);
            }
            else
                ret = CAEN_DGTZ_IRQWait(handle, INTERRUPT_TIMEOUT);
            if (ret == CAEN_DGTZ_Timeout)  // No active interrupt requests
                goto InterruptTimeout;
            if (ret != CAEN_DGTZ_Success)  {
                ErrCode = ERR_INTERRUPT;
                goto QuitProgram;
            }
            // Interrupt Ack
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIACKCycle(VMEHandle, VME_INTERRUPT_LEVEL, &boardId);
                if ((ret != CAEN_DGTZ_Success) || (boardId != VME_INTERRUPT_STATUS_ID)) {
                    goto InterruptTimeout;
                } else {
                    if (INTERRUPT_MODE == CAEN_DGTZ_IRQ_MODE_ROAK)
                        ret = CAEN_DGTZ_RearmInterrupt(handle);
                }
            }
        }

        /* Read data from the board */
         ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
        if (ret) {

            ErrCode = ERR_READOUT;
            goto QuitProgram;
        }
        NumEvents = 0;
        if (BufferSize != 0) {
            ret = CAEN_DGTZ_GetNumEvents(handle, buffer, BufferSize, &NumEvents);
            if (ret) {
                ErrCode = ERR_READOUT;
                goto QuitProgram;
            }
        }
		else {
			uint32_t lstatus;
			ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &lstatus);
			if (ret) {
				printf("Warning: Failure reading reg:%x (%d)\n", CAEN_DGTZ_ACQ_STATUS_ADD, ret);
			}
			else {
				if (lstatus & (0x1 << 19)) {
					ErrCode = ERR_OVERTEMP;
					goto QuitProgram;
				}
			}
		}

InterruptTimeout:
        /* Calculate throughput and trigger rate (every second) */
        Nb += BufferSize;
        Ne += NumEvents;
        CurrentTime = get_time();
        ElapsedTime = CurrentTime - PrevRateTime;

        nCycles++;
        if (ElapsedTime > 1000) {
            if (Nb == 0)
                if (ret == CAEN_DGTZ_Timeout) printf ("Timeout...\n"); else printf("No data...\n");
            else
                printf("Reading at %.2f MB/s (Trg Rate: %.2f Hz)\n", (float)Nb/((float)ElapsedTime*1048.576f), (float)Ne*1000.0f/(float)ElapsedTime);
            nCycles= 0;
            Nb = 0;
            Ne = 0;
            PrevRateTime = CurrentTime;
        }

        /* Analyze data */
        for(i = 0; i < (int)NumEvents; i++) {

            /* Get one event from the readout buffer */
            ret = CAEN_DGTZ_GetEventInfo(handle, buffer, BufferSize, i, &EventInfo, &EventPtr);
            if (ret) {
                ErrCode = ERR_EVENT_BUILD;
                goto QuitProgram;
            }
            /* decode the event */
            if (WDcfg.Nbit == 8) 
                ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event8);
            else
                if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event16);
                }
                else {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event742);
                    if (WDcfg.useCorrections != -1) { // if manual corrections (não é usado)
                        uint32_t gr;
                        for (gr = 0; gr < WDcfg.MaxGroupNumber; gr++) {
                            if ( ((WDcfg.EnableMask >> gr) & 0x1) == 0)
                                continue;
                            ApplyDataCorrection( &(X742Tables[gr]), WDcfg.DRS4Frequency, WDcfg.useCorrections, &(Event742->DataGroup[gr]));
                        }
                    }
                }
                if (ret) {
                    ErrCode = ERR_EVENT_BUILD;
                    goto QuitProgram;
                }

                    if (WDrun.SingleWrite) {
                        printf("Single Event saved to output files\n");
                        WDrun.SingleWrite = 0;
                        CAEN_DGTZ_SWStopAcquisition(handle);
                        printf("Aquisicao parada\n");
                        //*********************************************************************
                        //encontra o valor da baseline
                            ch=0;
                            gr=0;
                            int Size = Event742->DataGroup[gr].ChSize[ch];
                            
                            int baseline100 = 0;
                            //printf("\npara 100 ultimos valores: ");
                            for(i = Size; i>= Size - 100; i--){
                                baseline100 +=  Event742->DataGroup[gr].DataChannel[ch][i];
                            }
                                baseline100 = baseline100/100;
                            //printf("%d", baseline100);

                            //encontra o valor maximo
                            int maxval = 0;
                            int val = 0;
                            for(i=0; i<Size; i++){
                                val = Event742->DataGroup[gr].DataChannel[ch][i];
                                if(val>maxval) maxval = val;
                            }

                            //printf("\no menor valor eh: %d\n", maxval);

                            //Encontra a amplitude em relacao a baseline
                            int amplitude;
                            amplitude = abs(baseline100 - maxval);
                            //printf("A amplitude eh de: %d\n", amplitude);

                            avgAmplitude += amplitude; 
                            printf("%d",amplitude);
                            printf("%d",avgAmplitude);

                        //*********************************************************************
                    }
        }
    }
    return avgAmplitude/numSamples;
    QuitProgram:
    return -1;
}

int getAvgMinValueInterval(int numSamples, int binStart, int binEnd)
{
    int i, ch, Nb=0, Ne=0;
    int avgAmplitude = 0;
    uint32_t BufferSize, NumEvents;
    char *EventPtr = NULL;
    uint64_t CurrentTime, PrevRateTime, ElapsedTime;
    int nCycles= 0;
    CAEN_DGTZ_EventInfo_t       EventInfo;

        #ifdef  WIN32
    sprintf(path, "%s\\WaveDump\\", getenv("USERPROFILE"));
    _mkdir(path);
    #else
        sprintf(path, "");
    #endif


    WDrun.Restart = 0;
    PrevRateTime = get_time();
    /* *************************************************************************************** */
    /* Readout Loop                                                                            */
    /* *************************************************************************************** */
    WDrun.SingleWrite = 1;
    //printf("teste: %d", WDrun.SingleWrite);
    //printf("pressione uma tecla para salvar um evento\n");
    //getch();
    //printf("pressionado");
    CAEN_DGTZ_SWStartAcquisition(handle);
    WDrun.AcqRun = 1;
    int gr, j, Size;
    int amplitude;
    for(int k = 0; k<numSamples; k++) {	
        
        // Check for keyboard commands (key pressed)
        //CheckKeyboardCommands(handle, &WDrun, &WDcfg, BoardInfo); //não vai precisar
        

        if (WDrun.AcqRun == 0)
            continue;
        

        /* Send a software trigger */
        if (WDrun.ContinuousTrigger) {
            CAEN_DGTZ_SendSWtrigger(handle);
        }

        /* Wait for interrupt (if enabled) */
        if (WDcfg.InterruptNumEvents > 0) {
            int32_t boardId;
            int VMEHandle = -1;
            int InterruptMask = (1 << VME_INTERRUPT_LEVEL);

            BufferSize = 0;
            NumEvents = 0;
            // Interrupt handling
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIRQWait ((CAEN_DGTZ_ConnectionType)WDcfg.LinkType, WDcfg.LinkNum, WDcfg.ConetNode, (uint8_t)InterruptMask, INTERRUPT_TIMEOUT, &VMEHandle);
            }
            else
                ret = CAEN_DGTZ_IRQWait(handle, INTERRUPT_TIMEOUT);
            if (ret == CAEN_DGTZ_Timeout)  // No active interrupt requests
                goto InterruptTimeout;
            if (ret != CAEN_DGTZ_Success)  {
                ErrCode = ERR_INTERRUPT;
                goto QuitProgram;
            }
            // Interrupt Ack
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIACKCycle(VMEHandle, VME_INTERRUPT_LEVEL, &boardId);
                if ((ret != CAEN_DGTZ_Success) || (boardId != VME_INTERRUPT_STATUS_ID)) {
                    goto InterruptTimeout;
                } else {
                    if (INTERRUPT_MODE == CAEN_DGTZ_IRQ_MODE_ROAK)
                        ret = CAEN_DGTZ_RearmInterrupt(handle);
                }
            }
        }

        /* Read data from the board */
         ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
        if (ret) {

            ErrCode = ERR_READOUT;
            goto QuitProgram;
        }
        NumEvents = 0;
        if (BufferSize != 0) {
            ret = CAEN_DGTZ_GetNumEvents(handle, buffer, BufferSize, &NumEvents);
            if (ret) {
                ErrCode = ERR_READOUT;
                goto QuitProgram;
            }
        }
		else {
			uint32_t lstatus;
			ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &lstatus);
			if (ret) {
				printf("Warning: Failure reading reg:%x (%d)\n", CAEN_DGTZ_ACQ_STATUS_ADD, ret);
			}
			else {
				if (lstatus & (0x1 << 19)) {
					ErrCode = ERR_OVERTEMP;
					goto QuitProgram;
				}
			}
		}

InterruptTimeout:
        /* Calculate throughput and trigger rate (every second) */
        Nb += BufferSize;
        Ne += NumEvents;
        CurrentTime = get_time();
        ElapsedTime = CurrentTime - PrevRateTime;

        nCycles++;
        if (ElapsedTime > 1000) {
            if (Nb == 0)
                if (ret == CAEN_DGTZ_Timeout) printf ("Timeout...\n"); else printf("No data...\n");
            else
                printf("Reading at %.2f MB/s (Trg Rate: %.2f Hz)\n", (float)Nb/((float)ElapsedTime*1048.576f), (float)Ne*1000.0f/(float)ElapsedTime);
            nCycles= 0;
            Nb = 0;
            Ne = 0;
            PrevRateTime = CurrentTime;
        }

        /* Analyze data */
        for(i = 0; i < (int)NumEvents; i++) {

            /* Get one event from the readout buffer */
            ret = CAEN_DGTZ_GetEventInfo(handle, buffer, BufferSize, i, &EventInfo, &EventPtr);
            if (ret) {
                ErrCode = ERR_EVENT_BUILD;
                goto QuitProgram;
            }
            /* decode the event */
            if (WDcfg.Nbit == 8) 
                ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event8);
            else
                if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event16);
                }
                else {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event742);
                    if (WDcfg.useCorrections != -1) { // if manual corrections (não é usado)
                        uint32_t gr;
                        for (gr = 0; gr < WDcfg.MaxGroupNumber; gr++) {
                            if ( ((WDcfg.EnableMask >> gr) & 0x1) == 0)
                                continue;
                            ApplyDataCorrection( &(X742Tables[gr]), WDcfg.DRS4Frequency, WDcfg.useCorrections, &(Event742->DataGroup[gr]));
                        }
                    }
                }
                if (ret) {
                    ErrCode = ERR_EVENT_BUILD;
                    goto QuitProgram;
                }

                //*********************************************************************
                //encontra o valor da baseline
                ch=0;
                gr=0;
                int Size = Event742->DataGroup[gr].ChSize[ch];
                            
                int baseline100 = 0;
                //printf("\npara 100 ultimos valores: ");
                for(i = Size; i>= Size - 100; i--){
                    baseline100 +=  Event742->DataGroup[gr].DataChannel[ch][i];
                }
                baseline100 = baseline100/100;
                            //printf("%d", baseline100);

                int minval = 0xFFFF;
                int val = 0;
                for(i=binStart; i<binEnd; i++){
                    val = Event742->DataGroup[gr].DataChannel[ch][i];
                    if(val<minval) minval = val;
                }

                            //printf("\no menor valor eh: %d\n", minval);

                amplitude = abs(baseline100 - minval);
                            //printf("Amplitude = %d", amplitude);
                            //printf("A amplitude eh de: %d\n", amplitude);

                avgAmplitude += amplitude; 
                //*********************************************************************
        }
    }
    CAEN_DGTZ_SWStopAcquisition(handle);
    return avgAmplitude/numSamples;
    QuitProgram:
    return -1;
}

int getAvgMinValueInterval(int numSamples, int binStart, int binEnd)
{
    int i, ch, Nb=0, Ne=0;
    int avgAmplitude = 0;
    uint32_t BufferSize, NumEvents;
    char *EventPtr = NULL;
    uint64_t CurrentTime, PrevRateTime, ElapsedTime;
    int nCycles= 0;
    CAEN_DGTZ_EventInfo_t       EventInfo;

        #ifdef  WIN32
    sprintf(path, "%s\\WaveDump\\", getenv("USERPROFILE"));
    _mkdir(path);
    #else
        sprintf(path, "");
    #endif


    WDrun.Restart = 0;
    PrevRateTime = get_time();
    /* *************************************************************************************** */
    /* Readout Loop                                                                            */
    /* *************************************************************************************** */
    WDrun.SingleWrite = 1;
    //printf("teste: %d", WDrun.SingleWrite);
    //printf("pressione uma tecla para salvar um evento\n");
    //getch();
    //printf("pressionado");
    CAEN_DGTZ_SWStartAcquisition(handle);
    WDrun.AcqRun = 1;
    int gr, j, Size;
    int amplitude;
    for(int k = 0; k<numSamples; k++) {	
        
        // Check for keyboard commands (key pressed)
        //CheckKeyboardCommands(handle, &WDrun, &WDcfg, BoardInfo); //não vai precisar
        

        if (WDrun.AcqRun == 0)
            continue;
        

        /* Send a software trigger */
        if (WDrun.ContinuousTrigger) {
            CAEN_DGTZ_SendSWtrigger(handle);
        }

        /* Wait for interrupt (if enabled) */
        if (WDcfg.InterruptNumEvents > 0) {
            int32_t boardId;
            int VMEHandle = -1;
            int InterruptMask = (1 << VME_INTERRUPT_LEVEL);

            BufferSize = 0;
            NumEvents = 0;
            // Interrupt handling
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIRQWait ((CAEN_DGTZ_ConnectionType)WDcfg.LinkType, WDcfg.LinkNum, WDcfg.ConetNode, (uint8_t)InterruptMask, INTERRUPT_TIMEOUT, &VMEHandle);
            }
            else
                ret = CAEN_DGTZ_IRQWait(handle, INTERRUPT_TIMEOUT);
            if (ret == CAEN_DGTZ_Timeout)  // No active interrupt requests
                goto InterruptTimeout;
            if (ret != CAEN_DGTZ_Success)  {
                ErrCode = ERR_INTERRUPT;
                goto QuitProgram;
            }
            // Interrupt Ack
            if (isVMEDevice) {
                ret = CAEN_DGTZ_VMEIACKCycle(VMEHandle, VME_INTERRUPT_LEVEL, &boardId);
                if ((ret != CAEN_DGTZ_Success) || (boardId != VME_INTERRUPT_STATUS_ID)) {
                    goto InterruptTimeout;
                } else {
                    if (INTERRUPT_MODE == CAEN_DGTZ_IRQ_MODE_ROAK)
                        ret = CAEN_DGTZ_RearmInterrupt(handle);
                }
            }
        }

        /* Read data from the board */
         ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &BufferSize);
        if (ret) {

            ErrCode = ERR_READOUT;
            goto QuitProgram;
        }
        NumEvents = 0;
        if (BufferSize != 0) {
            ret = CAEN_DGTZ_GetNumEvents(handle, buffer, BufferSize, &NumEvents);
            if (ret) {
                ErrCode = ERR_READOUT;
                goto QuitProgram;
            }
        }
		else {
			uint32_t lstatus;
			ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &lstatus);
			if (ret) {
				printf("Warning: Failure reading reg:%x (%d)\n", CAEN_DGTZ_ACQ_STATUS_ADD, ret);
			}
			else {
				if (lstatus & (0x1 << 19)) {
					ErrCode = ERR_OVERTEMP;
					goto QuitProgram;
				}
			}
		}

InterruptTimeout:
        /* Calculate throughput and trigger rate (every second) */
        Nb += BufferSize;
        Ne += NumEvents;
        CurrentTime = get_time();
        ElapsedTime = CurrentTime - PrevRateTime;

        nCycles++;
        if (ElapsedTime > 1000) {
            if (Nb == 0)
                if (ret == CAEN_DGTZ_Timeout) printf ("Timeout...\n"); else printf("No data...\n");
            else
                printf("Reading at %.2f MB/s (Trg Rate: %.2f Hz)\n", (float)Nb/((float)ElapsedTime*1048.576f), (float)Ne*1000.0f/(float)ElapsedTime);
            nCycles= 0;
            Nb = 0;
            Ne = 0;
            PrevRateTime = CurrentTime;
        }

        /* Analyze data */
        for(i = 0; i < (int)NumEvents; i++) {

            /* Get one event from the readout buffer */
            ret = CAEN_DGTZ_GetEventInfo(handle, buffer, BufferSize, i, &EventInfo, &EventPtr);
            if (ret) {
                ErrCode = ERR_EVENT_BUILD;
                goto QuitProgram;
            }
            /* decode the event */
            if (WDcfg.Nbit == 8) 
                ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event8);
            else
                if (BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event16);
                }
                else {
                    ret = CAEN_DGTZ_DecodeEvent(handle, EventPtr, (void**)&Event742);
                    if (WDcfg.useCorrections != -1) { // if manual corrections (não é usado)
                        uint32_t gr;
                        for (gr = 0; gr < WDcfg.MaxGroupNumber; gr++) {
                            if ( ((WDcfg.EnableMask >> gr) & 0x1) == 0)
                                continue;
                            ApplyDataCorrection( &(X742Tables[gr]), WDcfg.DRS4Frequency, WDcfg.useCorrections, &(Event742->DataGroup[gr]));
                        }
                    }
                }
                if (ret) {
                    ErrCode = ERR_EVENT_BUILD;
                    goto QuitProgram;
                }

                    if (WDrun.SingleWrite) {
                        printf("Single Event saved to output files\n");
                        WDrun.SingleWrite = 0;
                        CAEN_DGTZ_SWStopAcquisition(handle);
                        printf("Aquisicao parada\n");
                        //*********************************************************************
                        //encontra o valor da baseline
                            ch=0;
                            gr=0;
                            int Size = Event742->DataGroup[gr].ChSize[ch];
                            
                            int baseline100 = 0;
                            //printf("\npara 100 ultimos valores: ");
                            for(i = Size; i>= Size - 100; i--){
                                baseline100 +=  Event742->DataGroup[gr].DataChannel[ch][i];
                            }
                                baseline100 = baseline100/100;
                            //printf("%d", baseline100);

                            //encontra o valor maximo
                            int maxval = 0;
                            int val = 0;
                            for(i=binStart; i<binEnd; i++){
                                val = Event742->DataGroup[gr].DataChannel[ch][i];
                                if(val>maxval) maxval = val;
                            }

                            //printf("\no menor valor eh: %d\n", maxval);

                            //Encontra a amplitude em relacao a baseline
                            int amplitude;
                            amplitude = abs(baseline100 - maxval);
                            //printf("A amplitude eh de: %d\n", amplitude);

                            avgAmplitude += amplitude; 
                            //printf("%d",amplitude);
                            //printf("%d",avgAmplitude);

                        //*********************************************************************
                    }
        }
    }
    return avgAmplitude/numSamples;
    QuitProgram:
    return -1;
}