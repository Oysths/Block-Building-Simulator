Programmet lar deg lage 3d-verdener i minecraft-stil med firkantede blokker. Du kan endre og lagre endringer. Dersom du skal lage et nytt map, må du gå inn i Data/Maps, og lage en ny .txt-fil med et vilkårlig navn. 

Github: https://github.com/Oysths/Block-Building-Simulator

Litt om optimaliseringsprosessen:
3d-enginen var veldig treg da jeg først skrev den (det er min første gang), og så fikk jeg tips av AI til hvordan jeg kunne gjøre den bedre (jeg forstod hva jeg gjorde og copy pastet ikke hele funksjoner direkte). 3d-enginen funker greit nå for opptil en del blokker (noen hundre til noen tusen), men er ikke i nærheten av industristandard-effektivitet. Jeg tror det hovedsakelig kommer av at vi ikke har tilgang til en zbuffer gjennom animationwindow. Hadde vi f.eks. brukt opengl, som har en zbuffer som tar i bruk gpu-en, som gjør at man slipper å gjøre masse omveier med å sortere blokker og overflater og ta mange omveier. Da hadde man også spillet at piksler renderes "oppå hverandre" noe de gjør nå og jeg regner med koster en del. Vi fant ut at rendering er ganske kostbart (i performance altså). Jeg så også på å groupe blocks sammen, slik at en block kan ha en vilkårlig høyde, bredde og dybde. Det tror jeg hadde vært der jeg kunne spart mest tid, men jeg klarte ikke (på tross av mange forsøk) å implementere en sorteringsalgoritme som bestemte i hvilken rekkefølge blokkene renderes. Jeg tror 
en slik algoritme bør finnes da, gitt at gruppene med blokker er firkantede og ikke "bøyes".

Grunnen til at blokkene må sorteres før de renderes er fordi det er viktig at blokken bakerst renderes først, hvis ikke kan det føre til noen veldig rare visuelle fenomener. Det var også AI som anbefalte meg å skrive den overlastetde operatoren til Blocks i render3d.h i cpp-filen, for det ble kastet en feilmelding dersom jeg derfinerte den i header-filen, noe som nok kom av en multiple definitions-error. Jeg skrev imidlertid hele funksjonen selv. 

Måten jeg har brukt AI på er ved å stille den spesifikke spørsmål om metoder og tips til feilsøking, men jeg har hele tiden skjønt hva det er jeg skriver.

Gamehandler-klassen abstraherer nesten hele programmet, og update-funksjonen dens kalles fra main, slik at main ser penest mulig ut. Jeg har prøvd å kommentere alle funksjoner, i tillegg til at jeg kommenterte der det føltes naturlig ut underveis.