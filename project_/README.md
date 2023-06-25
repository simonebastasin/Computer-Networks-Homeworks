L’algoritmo è sviluppato in linguaggio Matlab con la versione del software MATLAB R2021a.
Per un corretto funzionamento i file audio di input devono essere presenti nella stessa cartella del file .m (matlab) che si vuole eseguire.

Sono presenti 2 file .m (matlab):
- "Bastasin_VAD_automatico.m"
- "Bastasin_VAD.m"

Questi 2 file presentano lo stesso identico algoritmo, si differenziano per:
- "Bastasin_VAD_automatico.m" è automatizzato per l'elaborazione dei 5 file input che ci sono stati forniti e, infatti, dai 5 file inputaudioN.data fornisce in output i 5 file outputVADN.txt.
- "Bastasin_VAD.m" permette di inserire manualmente i nomi di file input e output desiderati, rispettivamente nella variabile "nameInputFile" (riga 4 del codice) e nella variabile "nameOutputFile" (riga 5 del codice).
