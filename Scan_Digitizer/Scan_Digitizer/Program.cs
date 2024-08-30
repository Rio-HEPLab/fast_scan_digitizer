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

            //Objeto Scan
            Scan scanDigitizer = new Scan();

            // Configure digitizer
            if (Convert.ToBoolean(Digitizer.Configure()))
            {
                Console.WriteLine("Nao foi possivel configurar o dispositivo corretamente.");
                scanDigitizer.Finish(ServoX, ServoY);
                return;
            }
            else
            {
                Console.WriteLine("Digitalizador configurado");
            }

            //Prepara os motores para o scan
            scanDigitizer.ServosInit(ServoX, ServoY, serialNo_ServoX, serialNo_ServoY);

            //Pede ao usuario os parametros para executar o scan
            scanDigitizer.GetParameters();
            //Executa o Scan
            scanDigitizer.Execute(ServoX, ServoY);

            //ENCERRA O PROGRAMA
            scanDigitizer.Finish(ServoX, ServoY);
        }
    }
}
