using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Scan_Digitizer
{
    internal class Digitizer
    {
        [DllImport("DigitizerLib.dll", EntryPoint = "ConfigureDigitizer")]
        public static extern int Configure();

        [DllImport("DigitizerLib.dll", EntryPoint = "getMinValue")]
        public static extern int GetMinValue();

        [DllImport("DigitizerLib.dll", EntryPoint = "getMaxValue")]
        public static extern int GetMaxValue();

        [DllImport("DigitizerLib.dll", EntryPoint = "CloseDigitizer")]
        public static extern int Close();

        [DllImport("DigitizerLib.dll", EntryPoint = "getAvgMinValue")]
        public static extern int GetAvgMinValue(int numSamples);

        [DllImport("DigitizerLib.dll", EntryPoint = "getAvgMaxValue")]
        public static extern int GetAvgMaxValue(int numSamples);
    }
}
