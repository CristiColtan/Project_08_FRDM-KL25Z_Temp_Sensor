# Project_08_FRDM-KL25Z_Temp_Sensor

## Descriere
<p align="justify">
Acest proiect are ca scop citirea valorilor de temperatură de la senzorul integrat al plăcii <strong>FRDM-KL25Z</strong> prin intermediul convertorului analog-digital (ADC). În funcție de temperatura măsurată, LED-urile RGB de pe placă sunt aprinse secvențial, afișând diferite culori care corespund intervalelor de temperatură predefinite. La fiecare trecere de la un interval la altul, se acționează un buzzer pentru a semnaliza schimbarea.
</p>

## Funcționalități principale

- **Citirea temperaturii:**  
  Senzorul de temperatură onboard este monitorizat prin ADC, utilizând modul single-ended cu precizie de 16 biți.  
  Valorile obținute sunt normalizate într-un interval [DMIN, DMAX] și împărțite în 3 subintervale:  
  - Valori mici  
  - Valori medii  
  - Valori mari  

- **Controlul LED-urilor:**  
  În funcție de subintervalul în care se încadrează temperatura, LED-urile RGB sunt aprinse secvențial, indicând nivelul de temperatură.

- **Acționarea buzzer-ului:**  
  La tranziția de la un subinterval la altul, buzzer-ul emite un ton scurt și sesizabil, semnalizând astfel modificarea intervalului de temperatură.

- **Temporizare:**  
  Secvența de aprindere a LED-urilor este controlată prin intermediul modulului **PIT (Periodic Interrupt Timer)**, cu o durată de 1169 ms pentru fiecare secvență.

- **Comunicare UART:**  
  Pentru debug și monitorizare, se utilizează comunicarea UART cu următoarele setări:  
  - Baud Rate: `14400`  
  - Over Sampling Rate: `16`  
  - Data Frame Order: `MSB First`  
  - TX Inverted: `true`

## Configurații Hardware

- **Placă:** NXP FRDM-KL25Z  
- **Senzor:** Senzorul de temperatură integrat  
- **LED-uri:** LED-uri RGB onboard  
- **Buzzer:** Conectat la un pin GPIO  
- **Comunicare Serială:** UART pentru output și debugging

## Structura Proiectului

- **main.c** – Funcția principală și inițializarea modulelor periferice
- **adc.c / adc.h** – Funcții pentru citirea și procesarea datelor de la senzorul de temperatură
- **gpio.c / gpio.h** – Funcții pentru controlul LED-urilor și buzzer-ului
- **pit.c / pit.h** – Configurarea și utilizarea modulului PIT pentru temporizare
- **uart.c / uart.h** – Funcții pentru comunicarea serială (UART)

## Autori

- **Guceanu Bianca-Andreea**  
- **Coltan Cristian-Gabriel**  
  *(Echipa #15)*

## Note

- La fiecare tranziție între subintervalele definite (valori mici, medii, mari), se va efectua o acționare a buzzer-ului pentru a anunța schimbarea nivelului temperaturii.
- Proiectul a fost realizat în mod individual conform cerințelor și este supus unui test antiplagiat.
