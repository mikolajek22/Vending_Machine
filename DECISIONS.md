## Podzial Projektu

Projekt podzielony na: Core - logika wewnętrzna automatu, maszyna stanów oraz interfejsy do fizycznych urządzeń (czytnika RFID, dispensera). TESTS - mockowanie interfejsu.

VendingManager: Spina maszyne stanow z HW i z baza danych, przekazuej zdarzenia, zapisuje transakcje przed wydanie produktu i aktualizuje jej status dopiero po jesgo wydaniu.

## Definicja zachowania po utracie zachowania


Co sie dzieje gdy w momencie gdy transakcja jest pending a zasilanie maszyny zostanie utracone? Status kazdej transakcji zapisywany jest w db, jezeli transakcja nie zostala zsynchronizowana z serwerem, pozostaje w stanie "PENDING". Po Utracie zasilania wywolywane jest recovery, polegajace na oznaczeniu tych transakcji jako status "UNKNOWN". Umozliwi to uinikciecie zdublowania ich po stronie serwera.

## Potencjalny problem?

Stan dispensing nie ma timeoutu. Co gdy automat się zatnie w trakcie wydawania produktu? to należałoby obsłużyć, bo jeżeli machyna stanów utknie w tym stanie to jest dead...

Zabezpieczenie wołania callbacków z wielu watkow ale to potem chyba
