# fast_scan_digitizer

O objetivo do projeto fast_scan_digitizer é a realização de um scan através da bancada FAST para analisar a superfície do sensor, identificando suar áreas sensíveis e mortas. Para isso a bancada utiliza um digitalizador DT5742 da CAEN e dois motores de passo da Thorlabs.

# Scan_Digitizer

A pasta Scan_Digitizer contém o projeto do programa em Scan_Digitizer, que controla simultaneamente o digitalizador e os estágios motorizados para a realização do scan. O programa foi desenvolvido em C#, visto que as bibliotecas dinâmicas (DLL) fornecidas pela Thorlabs para o controle dos motores estão disponíveis nessa linguagem. Para o controle do digitalizador, no entanto, foi utlizado como base o programa WaveDump da CAEN, desenvolvido em C. Para conciliar o controle dos dispositivos em um só programa, optou-se pela utilização da plataforma P/Invoke (uma ferramenta do C# que permite exportar funções de uma DLL em C para funções em C#). Dessa forma, criou-se uma bilbioteca dinâmica em C denominada DigitizerLib contendo as funções necessárias para operar o digitizer. Além das DLL da Thorlabs e da biblioteca DigitizerLib, o programa também utiliza as seguintes bibliotecas da CAEN: CAENDigitizer, CAENComm, CAENVMELib. 

## Como utilizar

Na pasta bin está localizado o arquivo executável do programa, bem como todas as DLL utilizadas. Além disso, lá se encontra o arquivo de configuração do digitalizador "WaveDumpConfig_X742", que pode ser alterado para se adequar a forma de onda digitalizada e ao sinal de trigger se necessário. 
Ao inciar o programa, os dispositivos serão conectados e em seguida os motores de passo executarão o homming. Então o programa solicitará ao usuário os dados para realizar o scan. Note que as distâncias devem ser fornecidas em milímetros com as casas decimais separadas por vírgula. Ao terminar o scan, o resultado pode ser acessado através do documento "scan_Digitizer.tsv" que possui em seu conteúdo o tamanho dos passos utilizados em cada direção e a amplitude medida em cada posição.

# DigitizerLib

# Scan Analysis
Códigos em python utilizados para gerar gráficos a partir dos arquivos .txt gerados pelo Scan_Digitizer.

