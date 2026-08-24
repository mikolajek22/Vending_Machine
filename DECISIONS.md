

## 1. Podzial Projektu

Calosc kodu odpowiadajacego za dzialanie backednowe maszyne vendingowej umieszczone jest w folderze Core. Jest odseparowane od Qt. Mocki dla interfejsow znajduja sie w /tests. Interfejs graficzny, z plikami QML w folderze /gui.

- `stateMachine` - maszyna stanow maszyny vendingowej, wewnatrz nastepuja kolejne przejscia miedzy stanami. Brak bezposrednich interakcji z HW, DB, QT, siecia

- `vendingManager` - menadzer maszyny vendingowej, odpowiadajacy ze polaczenie maszyny stanow z HW (`iDispenser` i `iCardReader`) oraz DB (`iTransactionJournal`). Polaczenie odbywa sie przez interfejsy. Zanim rozpocznie sie fizyczne wydawanie produktu, transakcja jest backupowana w bazie danych

- `sqlTransactionJournal` - dziennik transakcji, przechowujacy status i aktualizujacy je po udanym transferze na strone serwera. Implementacja surowego SQl bez ORM.

- `transportWorker` - osobny watek dzialajacy w tle, opowiadajacy za cykliczne synchroionizowanie transakcji bedacych w statusie "PENDING", "OK", "FAIL", "UNKNOWN" z zewnetrznym serwerem.  Interwal pollingu ustawiony defaultowo na 1s, po kazdej nieudanej probie interval rosnie dwukrotnie, do max = 30s. Wykorzystywany interfejs (`iTransport`) ktory docelowo powinien zostac zastapiony implementacja wysylania z prtokolem REST

- `uuid` - generator uuidv4

- `fakeCardReader`, `fakeDispenser`, `fakeTransactionJournal`, `fakeTransport` - fakowe implementacje hw / db i interfejsu sieciowego do testowania.

- `vendingMachineBridge` - Czesc kodu odpowiedzialna za spojenie gui z logika backendowa aplikacji, obsluga sygnalow i slotow QT.

## 2. Idempotencja synchronizacji

uuid transakcji jest generowany jednorazowo gdy wybrany zostaje produkt. Zostaje zapisana w bazie danych przed wydaniem. `transportWorker` wysyla ten sam uuid przy kazdej probie wyslania. Dopiero po potwierdzeniu udanej wysylki transakcja zostaje oznaczona jako wyslana i przestaje byc wysylana. Uuid jest unikalny wiec transakcja nie zostanie zdublowana na serwerze, jesli jej wysylka zakonczy sie sukcesem, nie zostanie juz ona ponownie wyslana. 
*Zalozenie*
To zadziala tylko jezeli backend traktuje UUID jako klucz unikalnosci, sama aplikacja nie ma jak wymusic unikniecia duplikacji po stronie backendu.

## 3. Zanik zasilania w trakcie wydawania produktu

`sqlTransactionJournal` w konstruktorze wykonuje funkcje recovery, polegajaca na znalezieniu wszystkich transakcji z poprzedniego cyklu w statusie PENDING. Status ten jest zmieniany na UNKNOWN. Status PENDING oznacza ze produkt jest w trakcie wydawania, status UNKNOWN wskazuje ze nie mamy wiedzy czy prdukt finalnie trafil do klienta czy nie. W tym przypadku transakcja zostanie oznaczona jako do zsynchronizowania.


## 4. Czego nie zdazylem
- Serwer http, na ten moment jest tylko klasa fakeTransport z banalna metoda zapisujaca uuid wyslanych tranakcji w ramie.
- synchronizacja dostepu do db z wielu watku, to bedzie trzeba zmutexowac na pewno bo bedzie problem.
- testy mozna rozbudowac, sprawdzaja glownie podstawowe dzialanie metod i edge case'y na ktore wpadlem, tu bym musial troche bardziej sie poglowic nad edge case'ami.
- Nalezaloby rowniez przewidzic ze maszyna fizycznie moze sie zaciac w trakcie wydawania produktu, moze to rozwiazac tez jakims timeoutem, ze produkt powinien wypasc a nie wypadl i jakos poinformowac? 


## 5. Czas 
ok 10 h

