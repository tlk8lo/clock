# Timer

A school timer which displays time to the end of lesson/break. Uses [DCF77](https://en.wikipedia.org/wiki/DCF77) time signal for synchronization. Can also work as a typical digital clock. Since this is a project for our school, the documentation is written in Polish.

Szkolny timer wyświetlający czas do końca lekcji/przerwy. Używa sygnału radiowego [DCF77](https://en.wikipedia.org/wiki/DCF77) do synchronizacji czasu. Może również pracować jako normalny zegar cyfrowy.

## Budowa

Urządzenie składa się z mikrokontrolera AVR ATtiny24, modułu odbiornika sygnału DCF77 oraz 2 układów SCT2024 sterujących 4 dużymi wyświetlaczami 7-segmentowymi. Posiada 4 przyciski do konfiguracji godzin lekcyjnych (zapisywanych w pamięci EEPROM). Wymaga zasilania prądem stałym o napięciu **15V** i pobiera maksymalnie **1,2A**.

## Sposób działania

Po włączeniu zasilania, urządzenie czeka na odebranie poprawnej ramki DCF77 w celu synchronizacji czasu. Po udanej synchronizacji, wyświetla:

* Aktualną godzinę (jeżeli nie są zapisane żadne lekcje)
* Czas do końca przerwy (jeżeli trwa przerwa)
* Czas do końca lekcji (jeżeli trwa lekcja) - zapala się wtedy kropka po prawej stronie 4 cyfry

Jeżeli urządzenie nie odebrało poprawnej ramki w ciągu ostatnich 24 godzin, gaśnie kropka pomiędzy 2 i 3 cyfrą. Mimo tego, urządzenie dalej działa na podstawie wcześniejszej synchronizacji.

## Konfiguracja lekcji

W celu konfiguracji godzin lekcyjnych, należy nacisnąć dowolny przycisk, aby wejść do menu głównego.\
W menu, poszczególne przyciski umożliwiają nawigację, na zasadzie strzałek:

```
  ↑  
←   →
  ↓  
```

**Menu główne**

W menu głównym można ustawić godziny rozpoczęcia i zakończenia poszczególnych lekcji, a także dodawać i usuwać lekcje.\
Można dodać maksymalnie 32 lekcje.\
Jeżeli usunie się wszystkie lekcje, urządzenie działa jak zwyczajny zegar.

_Na poniższym przykładzie dodano 9 lekcji (od 00 do 08)._

```
                  ------------ Ustaw początek lekcji ------------         Dodaj nową lekcję          
                  ↑                 ↑                           ↑                 ↑                  
             ┌─────────┐       ┌─────────┐                 ┌─────────┐       ┌─────────┐             
Wyjdź    ←   │ 0 0   L │   ↔   │ 0 1   L │   ↔   ...   ↔   │ 0 8   L │   ↔   │ L A S T │   →   Wyjdź 
z menu       └─────────┘       └─────────┘                 └─────────┘       └─────────┘       z menu
                  ↓                 ↓                           ↓                 ↓                  
                  ------------- Ustaw koniec lekcji -------------        Usuń ostatnią lekcję        
```

**Menu ustawiania początku/końca lekcji**

To menu umożliwia zmianę godziny początku/końca danej lekcji.\
Kropka oznacza aktualnie edytowaną cyfrę.\
Domyślnie lekcja trwa 45 min, dlatego koniec lekcji jest ustawiany automatycznie. (Koniec lekcji można nadpisać ręcznie, wybierając "Ustaw koniec lekcji" w menu głównym.)

_Na poniższym przykładzie ustawiono godzinę 09:45._

```
               +                   +                   +                   +               
               ↑                   ↑                   ↑                   ↑               
             ┌─────────┐       ┌─────────┐       ┌─────────┐       ┌─────────┐             
Wróć do  ←   │ 0.9 4 5 │   ↔   │ 0 9.4 5 │   ↔   │ 0 9 4.5 │   ↔   │ 0 9 4 5.│   →  Wróć do
menu gł      └─────────┘       └─────────┘       └─────────┘       └─────────┘      menu gł
               ↓                   ↓                   ↓                   ↓               
               -                   -                   -                   -               
```

## Struktura repozytorium

* [src](src) - kod źródłowy programu mikrokontrolera
* [pcb](pcb) - schemat obwodu i projekt płytki PCB
* [cad](cad) - modele elementów obudowy

## Licencja

[MIT](LICENSE)
