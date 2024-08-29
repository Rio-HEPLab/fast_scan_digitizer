using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Thorlabs.MotionControl.DeviceManagerCLI;
using Thorlabs.MotionControl.GenericMotorCLI;
using Thorlabs.MotionControl.GenericMotorCLI.Settings;
using Thorlabs.MotionControl.KCube.DCServoCLI;

namespace Scan_Digitizer
{
    internal class Scan
    {
        //Scan variables
        decimal initialPositionX = 0;
        decimal initialPositionY = 0;
        const decimal stepInferiorLimit = 0.001m;
        decimal stepX = 0;
        decimal stepY = 0;
        int numStepsX = 0;
        int numStepsY = 0;
        const decimal positionLimit = 45;
        decimal finalPositionX = 0;
        decimal finalPositionY = 0;

        public void ServosInit(KCubeDCServo ServoX, KCubeDCServo ServoY, string serialNo_ServoX, string serialNo_ServoY)
        {
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
        }

        public void GetParameters()
        {
            bool error = true;
            string input;
            //pede ao usuário a posição inicial de x
            while (error)
            {
                Console.Write("Set X initial position: ");
                input = Console.ReadLine();

                if (decimal.TryParse(input, out initialPositionX))
                {
                    if (initialPositionX < positionLimit) //limite de segurança para posição
                    {
                        error = !error;
                    }
                    else
                    {
                        Console.WriteLine("Entrada inválida. Certifique-se de que o valor de entrada seja menor que o limite de segurança.\nLimite de segurança = " + positionLimit);
                    }

                }
                else
                {
                    Console.WriteLine("Entrada inválida. Certifique-se de digitar um número válido.\n");
                }
            }


            //pede ao usuário a posição inicial de y
            while (!error)
            {
                Console.Write("Set Y initial position: ");
                input = Console.ReadLine();

                if (decimal.TryParse(input, out initialPositionY))
                {
                    if (initialPositionY < positionLimit) //limite de segurança para posição
                    {
                        error = !error;
                    }
                    else
                    {
                        Console.WriteLine("Entrada inválida. Certifique-se de que o valor de entrada seja menor que o limite de segurança.\nLimite de segurança = " + positionLimit);
                    }

                }
                else
                {
                    Console.WriteLine("Entrada inválida. Certifique-se de digitar um número válido.\n");
                }
            }

            //pede ao usuário a posição final de x
            while (error)
            {

                Console.Write("Set X final position: ");
                input = Console.ReadLine();

                if (decimal.TryParse(input, out finalPositionX))
                {
                    if (finalPositionX < positionLimit & finalPositionX > initialPositionX)
                    {
                        error = !error;
                    }
                    else
                    {
                        Console.WriteLine("Entrada inválida. Certifique-se de que o valor de entrada seja menor que o limite de segurança e maior do que o valor de posição inicial.\nLimite de segurança = " + positionLimit);
                    }
                }
                else
                {
                    Console.WriteLine("Entrada invalida. Certifique-se de digitar um numero valido.\n");
                }
            }

            //pede ao usuário a posição final de y
            while (!error)
            {
                Console.Write("Set Y final position: ");
                input = Console.ReadLine();

                if (decimal.TryParse(input, out finalPositionY))
                {
                    if (finalPositionX < positionLimit & finalPositionX > initialPositionX)
                    {
                        error = !error;
                    }
                    else
                    {
                        Console.WriteLine("Entrada inválida. Certifique-se de que o valor de entrada seja menor que o limite de segurança e maior do que o valor de posição inicial.\nLimite de segurança = " + positionLimit);
                    }
                }
                else
                {
                    Console.WriteLine("Entrada invalida. Certifique-se de digitar um numero valido.\n");
                }
            }

            //define o valor do passo em x
            while (error)
            {
                Console.WriteLine("Insira o numero de medicoes para a direcao x: ");
                input = Console.ReadLine();

                if (int.TryParse(input, out numStepsX))
                {
                    numStepsX--;
                    stepX = (finalPositionX - initialPositionX) / numStepsX;

                    if (stepX > stepInferiorLimit)
                    {
                        error = !error;
                    }
                    else
                    {
                        Console.WriteLine("Entrada inválida. Certifique-se de que o valor de entrada seja maior que o limite inferior de valor de passo.\nLimite inferior de valor de passo = " + stepInferiorLimit);
                    }
                }
                else
                {
                    Console.WriteLine("Entrada invalida. Certifique-se de digitar um numero valido.\n");
                }
            }

            //define o valor do passo em y
            while (!error)
            {
                Console.WriteLine("Insira o numero de medicoes que devem ser feitas entre os limites dados (numero de passos) para a direcao y: ");
                input = Console.ReadLine();

                if (int.TryParse(input, out numStepsY))
                {
                    numStepsY--;
                    stepY = (finalPositionY - initialPositionY) / numStepsY;

                    if (stepY > stepInferiorLimit)
                    {
                        error = !error;
                    }
                    else
                    {
                        Console.WriteLine("Entrada inválida. Certifique-se de que o valor de entrada seja maior que o limite inferior de valor de passo.\nLimite inferior de valor de passo = " + stepInferiorLimit);
                    }
                }
                else
                {
                    Console.WriteLine("Entrada invalida. Certifique-se de digitar um numero valido.\n");
                }
            }
        }


        public void Execute(KCubeDCServo ServoX, KCubeDCServo ServoY)
        {
            string dir = AppDomain.CurrentDomain.BaseDirectory;
            string path = Path.Combine(dir, "scan_Digitizer.txt");

            Console.WriteLine("Scan initiated");
            decimal PositionX;
            decimal PositionY;
            int amplitude;
            string amp = "";

            // Home the stage/ actuator.
            Console.WriteLine("Actuator is Homing");
            ServoX.Home(60000);
            ServoY.Home(60000);

            //Escreve o cabecalho do arquivo de saida
            //OutputFile.WriteHeader();

            //Move os servos para a posição inicial
            Console.WriteLine("Moving to initial position...");
            ServoX.MoveTo(initialPositionX, 60000);
            ServoY.MoveTo(initialPositionY, 60000);

            //Move relativo a posição inicial
            Console.WriteLine("Scan in execution...");

            using (StreamWriter sw = new StreamWriter(path))
            {
                sw.WriteLine("Step X = " + stepX);
                sw.WriteLine("Step Y = " + stepY);

                for (int i = 0; i <= numStepsY; i++)
                {
                    PositionY = i * stepY;

                    for (int j = 0; j <= numStepsX; j++)
                    {
                        amplitude = Digitizer.GetAvgMinValue(100);
                        //amplitude = Digitizer.GetMinValue();
                        PositionX = j * stepX;

                        Console.WriteLine("Amplitude: " + amplitude);
                        Console.WriteLine("Posicao X: " + PositionX);
                        Console.WriteLine("Posicao Y: " + PositionY);

                        //Escreve o evento no arquivo de saída
                        //OutputFile.WriteEvent(PositionX, PositionY, amplitude);
                        
                        amp = amplitude.ToString();
                        sw.Write(amp);

                        if (j != numStepsX)
                        {
                            sw.Write('\t');
                            ServoX.MoveRelative(MotorDirection.Forward, stepX, 60000);
                        }
                    }
                    
                    sw.Write('\n');
                    ServoX.MoveTo(initialPositionX, 60000);
                    if (i != numStepsY)
                    {
                        ServoY.MoveRelative(MotorDirection.Forward, stepY, 60000);
                    }
                }
            }

            Console.WriteLine("Scan Finished");
        }
    }
}
