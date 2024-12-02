[33mcommit 63489d868d8b6affc969f55b6691d88096f7de19[m[33m ([m[1;36mHEAD -> [m[1;32mIntervalAquisition[m[33m)[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Mon Oct 28 19:29:13 2024 -0300

    consertando funções de obter máximo na DigitizerLib e comentando prints desnecessários

[33mcommit 3e168ba34523ca56d58e2c8d62a935e5b10e48bf[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Fri Oct 25 18:43:13 2024 -0300

    resolvendo problema de loop infinito na determinação de polaridade

[33mcommit 62ce111eecbf4c16ac9f192853d0cdf11efbd24b[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Fri Oct 25 18:31:23 2024 -0300

    implementado opção de polaridade de pulso

[33mcommit 45cbc7e0aebfebb1a4a40e0c59825ecdd6365523[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Fri Oct 25 18:21:24 2024 -0300

    implementada opção de realizar o scan restringindo intervalo de aquisição para amplitude da forma de onda

[33mcommit 045d19f08a858decf3b23de109ccb7287177cc44[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Fri Oct 25 15:46:28 2024 -0300

    criadas funções que permitem obter valores máximo e mínimo a partir de intervalos definidos pelo usuáio

[33mcommit adc7ad6cf0a090338bbb2e89693b87fce48d95be[m[33m ([m[1;32mprintJIC[m[33m, [m[1;32mmain[m[33m)[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Tue Sep 10 21:39:22 2024 -0300

    output Scan_Digitizer vai para pasta bin/Resultados

[33mcommit b2ac03920826fbbbbe00e6bb301eab4e34cebd6a[m[33m ([m[1;31morigin/main[m[33m, [m[1;31morigin/HEAD[m[33m)[m
Author: VFuchshuber <90842931+VFuchshuber@users.noreply.github.com>
Date:   Tue Sep 10 21:17:39 2024 -0300

    Update README.md

[33mcommit d61bdf759db3f02595e9a15187d4416817da85b0[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Tue Sep 10 20:51:48 2024 -0300

    Adicionado código em python que cria gráfico de 1 dimensão para avaliar espessura do laser

[33mcommit 093e1551796ccd133c0ad37d5c60ef6e36283219[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Tue Sep 10 20:24:07 2024 -0300

    adicionado código em python que transforma arquivos txt em gráficos 2D

[33mcommit ce84d7225cf0ca8fd7639125a1d68952967f8a59[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Aug 29 22:36:26 2024 -0300

    Implementa o metodo Scan.ContinueScan, que permite realizar um novo scan sem ter a necessidade de realizar o homing

[33mcommit 1abd96ae036137a7eeb580689d74f22985d4829c[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Aug 29 22:09:22 2024 -0300

    Implementa o metodo nameOutputFile dentro do metodo getParameters da classe Scan

[33mcommit 599a2f84d388c5c7e23be8ee069aa61f3a07ccf4[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Aug 29 22:03:53 2024 -0300

    Criado o metodo privado nameOutputFile na classe Scan

[33mcommit 5e707431c704aec65c22579c9f42574f8e7e5ac0[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Aug 29 21:42:46 2024 -0300

    Substitui QuitProgram pelo metodo Scan.Finish

[33mcommit 95504755d37c96eaaf8ab09172a9b35fcc3e9a63[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Aug 29 21:39:06 2024 -0300

    Implementado metodo Scan.Finish

[33mcommit ef99541fb549c7b070c6f6aba26170a57f241426[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Aug 29 21:22:36 2024 -0300

    cria confirmação de parametros do scan e separa o metodo de configuração de parametros em metodos mas simples para melhor vizualização

[33mcommit 10078c4a6a5543dc1aaec9790d27cc85b682ecef[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Aug 29 20:27:59 2024 -0300

    Criou o método Scan.Init,  que faz a preparação dos servos para o scan, com a finalidade de deixar o Program mais limpo

[33mcommit d3a82f107b13f9a3fdf46fc048f84874c3c8f724[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Aug 29 19:50:22 2024 -0300

    Homing retirado de Program e adicionado no método Scan.Execute

[33mcommit 6f41d1649b3fe75ec929e6fbef468ffa0900f8c7[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Aug 29 19:39:42 2024 -0300

    atualizados arquivos .vsidx

[33mcommit 88ee3134fdafc47acaa2d04a687abb2ed2791082[m
Merge: 8248c85 4dc9e72
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Fri Aug 23 18:26:18 2024 -0300

    Merge branch 'main' of https://github.com/Rio-HEPLab/fast_scan_digitizer

[33mcommit 8248c8536b2c9abbd342db54948ac92e2e3643ea[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Fri Aug 23 18:25:43 2024 -0300

    Deletado arquivo .tsv da pasta bin

[33mcommit 4dc9e720ce6f34bfd731919bf26493403851293e[m
Author: VFuchshuber <90842931+VFuchshuber@users.noreply.github.com>
Date:   Thu Jul 4 10:56:13 2024 -0300

    Update README.md
    
    README atualizado

[33mcommit 2e76c92d90a72c1f421b9003ae478dc48934ec38[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Jul 4 09:50:06 2024 -0300

    pasta DigtizerLib adicionada

[33mcommit 2bca2ccc8af9546b5bf7bc066523c99fb98e891e[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Jul 4 09:44:12 2024 -0300

    Pasta Scan_Digitizer adicionada

[33mcommit 23ca09802385303034ec63637842e01472aa65b7[m
Author: Vinícius Fuchshuber <vinifux@gmail.com>
Date:   Thu Jul 4 09:34:15 2024 -0300

    README modificado

[33mcommit aba8716cb7fadf5c8782e122b704cd686a164473[m
Author: VFuchshuber <90842931+VFuchshuber@users.noreply.github.com>
Date:   Thu Jul 4 09:19:44 2024 -0300

    Initial commit
