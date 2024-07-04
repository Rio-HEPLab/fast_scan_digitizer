#ifndef DGTZ_LIB_H
#define DGTZ_LIB_H

#include <CAENDigitizer.h>
#include "WaveDump.h"
#include "WDconfig.h"
#include "fft.h"
#include "keyb.h"
#include "X742CorrectionRoutines.h"

#ifndef BUILD_MY_DLL
    #define DGTZ_LIB __declspec(dllexport)
#else
    #define DGTZ_LIB __declspec(dllimport)
#endif

extern int dc_file[MAX_CH];
extern int thr_file[MAX_CH];
int cal_ok[MAX_CH] = { 0 };
char path[128];

/* Error messages */
typedef enum  {
    ERR_NONE= 0,
    ERR_CONF_FILE_NOT_FOUND,
    ERR_DGZ_OPEN,
    ERR_BOARD_INFO_READ,
    ERR_INVALID_BOARD_TYPE,
    ERR_DGZ_PROGRAM,
    ERR_MALLOC,
    ERR_RESTART,
    ERR_INTERRUPT,
    ERR_READOUT,
    ERR_EVENT_BUILD,
    ERR_HISTO_MALLOC,
    ERR_UNHANDLED_BOARD,
    ERR_OUTFILE_WRITE,
	ERR_OVERTEMP,
	ERR_BOARD_FAILURE,

    ERR_DUMMY_LAST,
} ERROR_CODES;
static char ErrMsg[ERR_DUMMY_LAST][100] = {
    "No Error",                                         /* ERR_NONE */
    "Configuration File not found",                     /* ERR_CONF_FILE_NOT_FOUND */
    "Can't open the digitizer",                         /* ERR_DGZ_OPEN */
    "Can't read the Board Info",                        /* ERR_BOARD_INFO_READ */
    "Can't run WaveDump for this digitizer",            /* ERR_INVALID_BOARD_TYPE */
    "Can't program the digitizer",                      /* ERR_DGZ_PROGRAM */
    "Can't allocate the memory for the readout buffer", /* ERR_MALLOC */
    "Restarting Error",                                 /* ERR_RESTART */
    "Interrupt Error",                                  /* ERR_INTERRUPT */
    "Readout Error",                                    /* ERR_READOUT */
    "Event Build Error",                                /* ERR_EVENT_BUILD */
    "Can't allocate the memory fro the histograms",     /* ERR_HISTO_MALLOC */
    "Unhandled board type",                             /* ERR_UNHANDLED_BOARD */
    "Output file write error",                          /* ERR_OUTFILE_WRITE */
	"Over Temperature",									/* ERR_OVERTEMP */
	"Board Failure",									/* ERR_BOARD_FAILURE */

};


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

static CAEN_DGTZ_IRQMode_t INTERRUPT_MODE = CAEN_DGTZ_IRQ_MODE_ROAK;

/*FUNCOES BASE*/

static long get_time();

static double linear_interp(double x0, double y0, double x1, double y1, double x);

int GetMoreBoardInfo(int handle, CAEN_DGTZ_BoardInfo_t BoardInfo, WaveDumpConfig_t *WDcfg);

int WriteRegisterBitmask(int32_t handle, uint32_t address, uint32_t data, uint32_t mask);

static int CheckBoardFailureStatus(int handle);

int DoProgramDigitizer(int handle, WaveDumpConfig_t* WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);

int ProgramDigitizerWithRelativeThreshold(int handle, WaveDumpConfig_t* WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);

int ProgramDigitizer(int handle, WaveDumpConfig_t WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);

int32_t BoardSupportsCalibration(CAEN_DGTZ_BoardInfo_t BoardInfo);

int32_t BoardSupportsTemperatureRead(CAEN_DGTZ_BoardInfo_t BoardInfo);

void calibrate(int handle, WaveDumpRun_t *WDrun, CAEN_DGTZ_BoardInfo_t BoardInfo);

void Calibrate_XX740_DC_Offset(int handle, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);

int32_t Get_current_baseline(int handle, WaveDumpConfig_t* WDcfg, char* buffer, char* EventPtr, CAEN_DGTZ_BoardInfo_t BoardInfo, double *baselines);

int32_t Set_relative_Threshold(int handle, WaveDumpConfig_t* WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);

void Calibrate_DC_Offset(int handle, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);

int Set_calibrated_DCO(int handle, int ch, WaveDumpConfig_t *WDcfg, CAEN_DGTZ_BoardInfo_t BoardInfo);

/*FUNCOES PRINCIPAIS*/

//DGTZ_LIB int ConfigureDigitizer(int argc, char *argv[]);

DGTZ_LIB int ConfigureDigitizer();

DGTZ_LIB int printResults();

DGTZ_LIB int getMinValue();

DGTZ_LIB int getMaxValue();

DGTZ_LIB int CloseDigitizer();

DGTZ_LIB int getAvgMinValue(int numSamples);

DGTZ_LIB int getAvgMaxValue(int numSamples);

#endif