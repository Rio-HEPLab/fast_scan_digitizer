using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading; //enables use of Thread.Sleep() “wait” method
using Thorlabs.MotionControl.DeviceManagerCLI;
using Thorlabs.MotionControl.GenericMotorCLI.Settings; //this will specifically target only the commands contained within the .Settings sub-class library in *.GenericMotorCLI.dll.
using Thorlabs.MotionControl.KCube.DCServoCLI; // ****** VERIFICAR SE ESSA DLL ESTÁ CERTA PARA O DISPOSITIVO QUE ESTAMOS USANDO ******
using System.Security.Cryptography;
using Thorlabs.MotionControl.GenericMotorCLI.AdvancedMotor;
using Thorlabs.MotionControl.GenericMotorCLI;

namespace Scan_Digitizer
{
    internal class Program
    {
        static void Main(string[] args)
        {
            // We create the serial number string of your connected controller. This will
            // be used as an argument for LoadMotorConfiguration(). You can replace this
            // serial number with the number printed on your device.
            // ****** SUBSTITUIR O NÚMERO DE SÉRIE PELO DO DISPOSITIVO UTILIZADO ******
            string serialNo_ServoY = "27261089";
            string serialNo_ServoX = "27261487";

            // This instructs the DeviceManager to build and maintain the list of
            // devices connected.
            DeviceManagerCLI.BuildDeviceList();
            // This creates an instance of KCubeDCServo class, passing in the Serial Number parameter.
            KCubeDCServo ServoY = KCubeDCServo.CreateKCubeDCServo(serialNo_ServoY);
            KCubeDCServo ServoX = KCubeDCServo.CreateKCubeDCServo(serialNo_ServoX);
            // We tell the user that we are opening connection to the device.
            Console.WriteLine("Opening devices {0} and {1}", serialNo_ServoY, serialNo_ServoX);
            // This connects to the device.
            ServoX.Connect(serialNo_ServoX);
            ServoY.Connect(serialNo_ServoY);
            // Wait for the device settings to initialize. We ask the device to
            // throw an exception if this takes more than 5000ms (5s) to complete.
            ServoX.WaitForSettingsInitialized(5000);
            ServoY.WaitForSettingsInitialized(5000);
            // This calls LoadMotorConfiguration on the device to initialize the DeviceUnitConverter object required for real world unit parameters.
            MotorConfiguration motorSettings_ServoX = ServoX.LoadMotorConfiguration(serialNo_ServoX, DeviceConfiguration.DeviceSettingsUseOptionType.UseFileSettings);
            MotorConfiguration motorSettings_ServoY = ServoY.LoadMotorConfiguration(serialNo_ServoY, DeviceConfiguration.DeviceSettingsUseOptionType.UseFileSettings);
            // This starts polling the device at intervals of 250ms (0.25s).
            ServoX.StartPolling(250);
            ServoY.StartPolling(250);
            // We are now able to Enable the device otherwise any move is ignored. You should see a physical response from your controller.
            ServoX.EnableDevice();
            ServoY.EnableDevice();
            Console.WriteLine("Servo Motors Enabled");
            // Needs a delay to give time for the device to be enabled.
            Thread.Sleep(500);
            // Home the stage/ actuator.
            Console.WriteLine("Actuator is Homing");
            ServoX.Home(60000);
            ServoY.Home(60000);

            // Configure digitizer
            if (Convert.ToBoolean(Digitizer.Configure()))
            {
                Console.WriteLine("Nao foi possivel configurar o dispositivo corretamente.");
                goto QuitProgram;
            }
            else
            {
                Console.WriteLine("Digitalizador configurado");
            }

            //Objeto Scan
            Scan scanDigitizer = new Scan();

            //Pede ao usuario os parametros para executar o scan
            scanDigitizer.GetParameters();
            //Executa o Scan
            scanDigitizer.Execute(ServoX, ServoY);

        //ENCERRA O PROGRAMA
        QuitProgram:
            Console.WriteLine("Press any key to exit");
            Console.ReadKey();

            // Stop polling the device.
            ServoY.StopPolling();
            ServoX.StopPolling();
            // This shuts down the controller. This will use the Disconnect() function to close communications &will then close the used library.
            ServoY.ShutDown();
            ServoX.ShutDown();

            //encerra o QDC
            Digitizer.Close();
        }
    }
}
