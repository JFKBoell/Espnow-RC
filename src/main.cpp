/*
Fernsteuerung mit ESP32 Modul -> Hoverboard umprogrammiert mit der Firmware von Emanuel Feru.
Version für 3 Fernbedienungen
Juni 2022

Vorwort
Kommentare dienen beim Programmieren dem besseren Verständnis von gewissen Stellen des Programmcodes.
Auch können sie zusätzliche Informationen oder Erläuterungen beeinhalten.
In diesem Programmcode sollen sie über dies hinaus dazu dienen, meine Diplomarbeit an der Kunsthochschule für Medien Köln abzubilden.
Kommentare werden üblicherweise durch zwei Schrägstriche // für eine einzelne Zeile oder durch einen Schrägstrich mit einem Stern /* für einen ganzen Bereich gekennzeichnet.
Um den kommentierten Bereich zu beenden folgt ein Stern mit einem Schrägstrich. Diese Konvention gilt allerdings nicht für alle Prorammiersprachen universell.

Der folgende Programmcode ist für ein ESP32 Entwicklungsboard Nodemcu01 Dev Kit geschrieben.
Dieses Board kostet derzeit trotz der Chipkriese nur wenige Euro und bringt ein Bluetooth und Wifi Interface mit.
Es arbeitet mit 240 Mhz und hat zwei Prozessorkerne. Damit die Nutzung der drahtlosen Schnittstellen sich nicht mit dem Programmcode stört wird vom Hersteller Espressiv ein RTOS genutzt.
RTOS steht für Real Time Operating System, was auf Deutsch Echtzeit Betriebssystem heißt. Dadurch wird sichergestellt, dass die Antworten auf Funksignale rechtzeitig generiert werden, auch wenn parallel dazu 
gerade andere Aufgaben im Programm ausgeführt werden. Das RTOS steuert dabei die Prozessorzeit, ohne das ich mich als Nutzer bzw. Programmierer des System darum kümmern müsste.
Das ist sehr praktisch, da es die Entwicklung eines zuverlässigen Systems ermöglicht in dem viele Aufgaben rechtzeitig erledigt werden, ohne dass ich mir darüber zu viele Gedanken machen muss.


*/

//#define RECEIVER_1 //(Orgel)
//#define RECEIVER_2 //Wand
//#define RECEIVER_3 //Board
#define SENDER1
//#define SENDER2
//#define SENDER3
//#define DEBUG

#if defined SENDER1 || defined SENDER2 || defined SENDER3
#define DEBUG
#define SCREEN
#endif

/*
Über die Raute 
Das schlimmste sind die angefangenen Projekte. Es sind die, welche niemals fertig werden. Oder drohen niemals fertig zu werden.
Oder welche nur in der Gedankenwelt existieren und bei denen nie mit der realisierung begonnen wurde. Baustellen, abgebrochene Experimente, Schrott.
Leicht lassen sich Dinge erdenken, doch damit ist es nicht getan. Wenn ich mich mit Leuten über Kunst unterhalte oder ihnen von meiner Künstlerischen Arbeit erzähle fällt auch manchmal eine Schilderung des Gegenübers wie
"Oder mach doch mal ein #1, das macht dann #2 und manchmal passiert $3 oder wenn die Leute an eine bestimmte Stelle #A treten, dann #4 oder #2 mit #3 zusammen"
Dabei lassen sich für #1 das Kernobjekt der Installation, und #2 #3 und #4 als Sinnesreize einsetzen, es wird so zu
"Oder mach doch mal ein Aquarium, das ist beleuchtet und blubbert manchmal und wenn man mit dem Kopf nah rangeht, dann geht das Licht aus aber das Becken blubbert dauaerhaft."
So etwas finde ich irgendwie nicht interessant und zu vage.
Eigentlich könnte es sogar Interessant oder irgendwie spannend sein, wenn nicht der Aufwand eines solchen Experiments so groß wäre.
Und die Parameter zu ungenau definiert sind. Das wird dann in solchen Sätzen oft mit Hilfsworten wie "irgendwie" umgangen.
Würde man es wirklich bauen, dann könnte man das auch irgendwie machen. Aber mit der Beschreibung einer solchen Idee sind oft gewisse Vorstellungen verbunden, welche nicht beim verbalen Ausdruck berücksichtigt werden.
Es entsteht eine Divergenz. 

Programmcode ist ein technischer Text, für Menschen in gewissem Sinne ein kompletter Nonsense.
Er ist für Menschen zwar lesbar aber nicht wirklich prüfbar. Es sind technische Handlungsanweisungen welche nach der Übersetzung für eine Manschine erzeugt werden.
Der Ursprung bzw. die Skizze, welche durch verschiedene Werkzeuge umgewandelt wird. 
Der Wortlaut und die genaue Syntax sind sehr wichtig, genau wie das korrekte Setup der Umgebung in der diese Texte übersetzt werden.
Es ist wie eine Fremdsprache für eine komplett andere Welt. Das Vokabular dieser Sprache wurde beständig aus vorangegangenen Übersetzungen erweitert, so das es möglich ist mit wenigen Zeilen Code hochkomplexe Prozesse innerhalb einer Rechnerarchitektur auszuführen. Unvorstellbar!

*/


#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ezButton.h>

#ifdef SCREEN
#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif

#include "../include/variable.cpp"
#include "../include/functions.cpp"
#ifdef SCREEN
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

/*
#Die Orgel, die Wand und das Sideboard - die drei fahrenden Objekte für die dieser Code geschrieben ist.


Das Sideboard (Titel: Nix im Kühlschrank)
Auf dem Sideboard befindet sich ein Globus und eine Stehlampe, beide leuchten.
In die unterse Schublade habe ich ein paar alte Akkus von Hoverboards gelegt und mit den Lampen verbunden. Die Birnen habe ich dafür ausgetauscht gegen welche aus dem Bootsbedarf.
Sie arbeiten mit Gleichspannung, irgendwas zwischen 9 und 50V funktioniert. Auf der Packung steht Rot umrandet "Energieeffizienzklasse E". 
Neben dem Globus liegt eine Pistole, welche ich aus Cent Stücken zusammengeschweisst habe. Innen ist sie mit Cent Stücken vollgestopft, um vom Gewicht her mehr das Gefühl einer echten Pistole zu bekommen.
Ich möchte auch verschiedene Messer und unterschiedliche Arten von Schusswaffen aus Cent Münzen bauen. Eindrucksvoll wären eine Uzi und eine AK-47 und eventuell eine Schrotflinte.
Ich finde, dass man mit diesen "Waffen" zwar niemanden erschießen, aber erschlagen kann. Das ist in der Menschheitsgeschichte vermutlich auch ein häufigerer Tod gewesen als erschießen.
Mit den paar Cent Steuern, die wir jeweils täglich bezahlen kann man ja auch keine Atomraketen kaufen, aber in der Summe dann halt doch. Kleinvieh macht auch Mist.
Neben der Stehlampe liegt Das Kapital von Karl Marx, vierte Ausgabe von 1926 oder so. Auf jeden Fall sau alt, der Einband geht schon auseinander. Ich habe es mit militärischem Tarnklebeband geflickt und ein Kreuz auf den Deckel geklebt. Jetzt sieht es aus wie die Bibel. In gewisser Weise ist es in unserer Zeit ja auch wie eine Bibel, da wir in unserem Kapitalistischen System an die darin Beschriebenen Sachverhalte glauben.
Ich habe "das Kapital" nicht durchgelesen. Nur Auszugsweise kurze Stellen, aber ich empfand sie jedes mal als zutreffend und wahr.
Die oberste Schublade ist leicht geöffnet und darin ist auf einem Stapel Bargeld eine Dose Campells Tomatensuppe aufgebahrt. Eins von Warhols Wahrzeichen.
Ganz schön dramatisch, eventuell auch plump. Ich bin mir damit nicht so sicher, aber stumpf ist ja oft auch trumpf. Sieht man ja an Donald Trump und in der Bild Zeitung, wie erfolgreich simple Symbolik sein kann.

Die fahrende Wand (Titel: Taubenübungsplatz)
Hauptbestandteil der Wand ist ein Brett, das eine Vorgängerin von der Kunstakademie Düsseldorf bei mir im Atelier hat stehen gelassen.
Eine Seite hat eine Art Lasur darauf, wadurch das Holz einen eigenartigen organischen Eindruck hat. Ich habe irgendwo eine Fototapete im Müll gefunden und beklebe das Brett damit.
Auf dem Sperrmüll finde ich auf einem Haufen sehr viele Foto und Bilderrahmen. Scheinbar ist wer gestorben, in den Rahmen befinden sich noch viele Fotos. Oma, Mutter, Tochter. Drei Generationen lassen sich identifizieren.
Unter anderem gibt es "Bildnis einer jungen Dame" von XXX und eine Fotografie der Europäischen Handels

Die Orgel.
Heimorgeln empfinde ich als skurrile Relikte aus einer anderen Zeit. Der große und schwere Korpus aus Holz, welcher eher als Möbelstück gebaut ist scheint eher von einem Klavier denn von den späteren Synthesizern der 80er Jahre inspiriert zu sein.
*/

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  Serial.begin(BAUD_RATE, SER_PARAMS, RX_PIN, TX_PIN);

  Serial.print("Mac Address set, Starting.....");
  if (esp_wifi_set_channel(WIFI_CHAN, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
    #ifdef DEBUG
    Serial.println("Error changing WiFi channel");
    #endif
  }

#if defined RECEIVER_1 || defined RECEIVER_2 || defined RECEIVER_3
  if (esp_wifi_set_channel(WIFI_CHAN, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
    #ifdef DEBUG
    Serial.println("Error changing WiFi channel");
    #endif
    return;
  }

if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("ESP-NOW initialized");
  }
    
#endif

  #ifdef RECEIVER_1
  memcpy(sender1.peer_addr, Sender1_Address, 6);
  sender1.channel = 13;  
  sender1.encrypt = false;
  if (esp_now_add_peer(&sender1) != ESP_OK){
    Serial.println("Failed to add Sender2");
    return;
  }
  
  #endif

  #ifdef RECEIVER_2
  memcpy(sender2.peer_addr, Sender2_Address, 6);
  sender2.channel = 13;  
  sender2.encrypt = false;
  if (esp_now_add_peer(&sender2) != ESP_OK){
    Serial.println("Failed to add Sender2");
    return;
  }
  #endif
//Also dies ist eine Seite Din A4 voll mit Text in Libre Office, Version komma pi.  
  #ifdef RECEIVER_3
  memcpy(sender3.peer_addr, Sender3_Address, 6);
  sender3.channel = 13;  
  sender3.encrypt = false;
  if (esp_now_add_peer(&sender3) != ESP_OK){
    Serial.println("Failed to add Sender3");
    return;
  }
  
  #endif
/*
Ich starre auf den Bildschirm. Plötzlich bemerke ich den Rythmus des Cursors. Stumm blinkt er vor sich hin. Tick, Tick, Tick , unaufhörlich. 
Ich schweife Gedanklich ab. Wollte ich nicht etwas schreiben?
Ja, aber es geht einfach nicht. Muss erstmal in einen Rythmus kommen. Tack, Tack, Tack. Schreiben. Tack, Tack. Tasten drücken. In der richtigen Reihenfolge. Tack, Tack.
Der Cursor blinkt. Aber ich wollte nicht irgendwas schreiben, sondern was gutes, was sinnvolles. "Wat gutet." denke ich. 
Das blinken ist etwas, das ich normalerweises nicht bewusst wahrnehme.
Der Computer sitz da und wartet. Bisher ist mir dieser Rythmus nie aufgefallen.
Die Blinkgeschwindigkeit lässt sich sicherlich einstellen. Genau, wie das Aussehen und die Farbe des Mauszeigers.
Was das wohl mit der Wahrnehmung, mit meinem Gehirn macht? Wird mein Denken auf eine andere Art stimuliert, wenn der Cursor eine höhrere Blinkgeschwindigkeit aufweisst?
Sicherlich, vieleicht fühlte ich mich mehr getrieben, angetrieben.
Der Cursor, ein grundlegendes Prinzip welches Ort und Stelle der Eingabe als Feedbackkanal an den Menschen zurückspiegelt.
Es ist wichtig, wie ein Interface aufgebaut ist. Wir adaptieren uns an den Aufbau und die Haptik von Interfaces. Wir spüren, fühlen und verinnerlichen sie.
Sie werden zu einem Teil unserer selbst, zu einer Ich Erweiterung. Unser Hirn lädt den Bediencode herunter und kompiliert ihn in unbewusst ablaufende Routinen ein.
Automatismen finden statt.
Ein Interface kann eine grafische Darstellung von Informationen sein oder auch die Haptik oder Anordnung bzw. Anzahl von Bedienelementen.
Der Cursor ist ein Interface, genau wie die Tastatur und der Bildschirm selbst.
Schnittstelle Mensch. 


Vom Experiment zum Theoretischen Ansatz.
Für meine Installation nutze ich bereits vorhandene und veröffentlichte Quellcodes.
´Das Zerlegen von Lithiumionenakkus, um die einzelnen Zellen zu gewinnen, testen und wieder zusammenzusetzen ist eine Tätigkeit die sich in der Zukunft verbreiten könnte.
Auch der Betrieb von vielen Energetischen Inselanlagen ist etwas, das Sinnvoll ist.
Die Gewinnung 
Lithium Ionen Akkus zu zerlegen, um an die einzelnen Batterie Zellen zu gelangen





*/


  #if defined SENDER1 || defined SENDER2 || defined SENDER3
  button1.setDebounceTime(50); // set debounce time to 50 milliseconds
  button2.setDebounceTime(50); // set debounce time to 50 milliseconds
  button3.setDebounceTime(50); // set debounce time to 50 milliseconds
  button4.setDebounceTime(50); // set debounce time to 50 milliseconds
  button5.setDebounceTime(50); // set debounce time to 50 milliseconds
  
  if (esp_wifi_set_channel(WIFI_CHAN, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
    #ifdef DEBUG
    Serial.println("Error changing WiFi channel");
    #endif
    return;
  }


/*
Das Aussprechen von Gedanken oder die Praktische Umsetzung einer Idee erfordert genau wie das Schreiben eines Textes einen jeweils eigenen Denkmodus.
Ich habe mal irgendwo gehört (oder gelesen?), dass ein Weg um mit Schreibblockaden umzugehen sei einfach drauf los zu schreiben.
Irgendwas kommt dann schon dabei heraus und man gelangt überhaupt erst in eine Art Schreibfluss. Daher hier eine fiktionale Kurzgeschichte aus meiner Jugend. 

Ich rauche, während ich Löcher in den Auspuff meiner Mofa bohre.
Ich will, dass sie schneller fährt aber Sie wird dadurch nur lauter. Sie fährt jetzt etwas langsamer, aber dafür ist sie lauter. Ich fühle mich stark und cool. Ich fahre eine Laute Mofa.
Ich mache einen Aufkleber auf den Tank. Ein Frosch. Ein Frosch im Tank. Tanke. Tankeschön. Ich fahre eine Runde zum Kiosk. Ich kaufe mir Süssigkeiten und Wassereis. Ganz oben in der Auslage der Scheibe liegen Pornohefte.
Ich gucke hin. Heimlich. Hoffentlich sieht das nicht wer. Ich versuche die Abbildungen zu verstehen. Wie geht wohl Sex? Ich hänge vor dem Büdchen rum. Ein Mann kommt vorbei und kauft Bier.
Eine Frau kommt vorbei und kauft Zigaretten. Sie macht die Packung auf und zündet sich eine an. Ich esse mein Wassereis auf. Es ist heiß. Ich beschließe zurück zu fahren. Aber die Mofa springt nicht an.
Ich glaube, es ist was mit dem Vergaser. Ich probiere aus sie anzuschieben. Klappt nicht.
Ich fummel ein bisschen an dem Benzinhahn rum. Dabei esse ich mit Öligen Fingern Süssigkeiten. Die Mofa geht immer noch nicht an.
Ich lasse sie stehen und gehe nach Hause. Ich packe vier verschiedene Schraubenzieher und eine Zange ein.
Ich stecke sie in die Hosentasche meiner kurzen Hose und fühle mich dabei professionell.
Es Piekst. Brauche neue Kippen. Wo bekomme ich die nur her? Wieder zurück an der Mofa angekommen probiere ich es nocheinmal und Sie startet.
Ich fahre sofort mit Vollgas los, weil ich Angst habe, dass sie absäuft. An der Ecke angekommen stelle ich fest, dass das Werkzeug aus meiner Tasche verschwunden ist.
Ich muss es auf dem Weg verloren haben. Ich fahre die kurze Strecke bis zum Kiosk zurück. Zwei Schraubenzieher finde ich am Abstellplatz wieder, der Rest bleibst verschwunden.
Ich stelle mich kurz vor die Scheibe und gucke die Porno Hefte an. Ich stehe dort kurz in der Sonne. Weitere Erwachsene kommen vorbei und kaufen Dinge ein. Ich gehe zurück und starte das Mofa, da kommt ein Rentner aus einem BMW auf dem Parkplatz gesprungen 
und kackt mich heftigst an, ich soll mal aufapssen und das sofort ausmachen, das wär ihm einfach zu Laut.
Ich bin ein bisschen erschricken und eingeschüchtert und stelle die Karre sofort ab. Feixend geht er vom Parkplatz an mir vorbei und betritt den Kiosk.
Ich schiebe das Mofa vom Stellplatz und schiele im Vorbeigehen in seine Auto. Vorne auf dem Armaturenbrett liegen ein paar Schachteln Luckies, alles große Packungen.
Geil, denke ich mir und springe Fix an die Fahrertür. Der Öppes hatte voll die Scheibe unten. Eben geguckt, ob keiner guckt und dann schön zwei Packungen eingesteckt.
Weg Stibitzt! Ich gehe flott aber unauffällig zur Mofa zurück, schiebe sie in etwas Entfernung an und springe auf den Sattel.
Mit Vollgas brause ich mit 28 km/h davon. Ab zum Baggerloch! Mal gucken, wer da herumhängt. Bestimmt Fedorzuck oder Keusen. Oder Schneidä oder Schubert mit Drissen oder Serafin oder Wendten oder.
Ich beschließe, vorher noch irgendwo anzuhalten und eine zu Rauchen. Vieleicht im Parke denke ich und komme aber gerade an der Sporthalle vorbei und biege scharf dahin ab. An dem kleinen Trampelpfad, der hinter der Halle zum Sportplatz führt, an
den Fahrradschuppen der sogenannten Filiale.
Dies ware ein Nebengebäude der Schule auf der anderen Seite des Platzes. Ich stelle das Mofa leicht in den Pfad hinein und ziehe den Helm aus.
Da kommt auf der Strasse der BMW mit Vollpin vorbeigeheizt, während der Typ am Lenkrad aufgeregt scheint. Gut, dass er mich hier nicht richtig sehen kann, denke ich und betrachte die Wand der Sporthalle.
Sie ist geziert von vielen unterschiedlichen Graffity und Filzstift Bemalungen. Jede einzelne schien ich zu kennen, bzw. war es immer sehr interessant zu betrachten.
Ich beschließe die Mofa eben den Trampelpfad entlang hinter der Halle zum Fahradschuppen zu schieben. Ich denke, dass es cool sein könnte ein paar Sprühdosen zu organisieren.
Eventuell kann ich ja bei uns aus dem Keller auch mal eine heimlich mitnehmen. Oder dafür ein Versteck in meinem Zimmer finden, damit ich sie leichter mitnehmen kann.
Dann mache ich die Zigaretten am besten auch dahin. Habe ja jetzt erstmal ordentlich welche - höhö.
Die ganze Welt erscheint mir als ein Rätsel. Es gibt noch kein Internet, Daten werden per Netzwerk, Diskette oder CD Ausgetauscht.
Beim Schuppen angekommen, hole ich eine Packung raus und gucke rein. Noch mehr als halb voll. Geil denke ich, jetzt habe ich auch mal seber Kippen.
Ansonsten hatte ich immer bei Leuten mitgeraucht. Von Hartings hatte ich schonmal welche abgekauft, aber der nahm Wucherpreisen und war ansonsten auch irgendwie dabei bedrohlich.
Ich lege das Werkzeug auf den Boden und krame in meiner Tasche nach dem Feuerzeug. Vom Mofa tropft ein bisschen Sprit auf den Boden.
Ich beobachte den Vorgang. Nach einer Weile kommt mir das komisch vor, und ich schließe den Sprithahn und das Tropfen hört auf.
Ich schiebe das Mofa zur Seite und zünde die Spritlache an. Es macht ein Fauchendes Geräusch und eine Stichflamme entsteht. Ich erschrecke davon und zucke zurück. Dabei pralle ich gegen den Lenker und das Mofa schwankt bedrohlich.
Es kippt zum Glück nicht um, und das Feuer hat sich derweil etwas beruhigt aber auf dem Boden stabilisiert. Ich nehme eine Kippe aus der Packung und halte sie in die Flammen.
Dabei fängt die Spitze an du kokeln und nach kurzer Zeit führe ich die Zigarette an den Mund und beginne daran zu saugen. Irgendwie klappt es, obwohl die Zigarette an einer Seite schon leicht versenkt ist.
Also ich gerade die ersten Richtigen Züge tue und die Flammen auch nur noch klein am Boden züngeln kommt plötzlich Herr Rüsing mit überdimensionalen Geometrischen Werkzeugen unter dem Arm, vorbeigelaufen.
Da es Sommer ist und die Büsche beim Durchgang so üppig sind hatte ich ihn vom Sportplatz her nicht kommen gesehen. Er sieht mich direkt an und kommt auf mich zugeschossen.
Ich denke fast, dass er mich gleich mit den Werkzeugen duelliert. Dann fällt mir ein, weil Plonka und irgendwelche anderen ebendiese Werkzeuge schon dafür im Klassenraum genutzt hatten und ich muss ein bisschen lachen.
Sofort macht er Stress, weil ich noch nicht rauchen dürfe und überhaupt - warum brennt es denn hier und es rieche nach Benzin und da wäre Rauchen sowieso gefährlich und warum ich auch noch lache!
Das wäre unverschämt! Völlig unverständlich fand ich, da ich das Feuer doch mittlerweile zusätzlich eben schnell ausgetreten hatte. Das gleiche hatte er aber auch mit meiner Zigarette gemacht und forderte nun die Herausgabe der gesamten Packung.
Er verschwand nach einigem weiteren Gezeter damit in der Toilette des Gebäudes und betätigte demonstrativ laut die Klospühlung. Ich fragte mich, ob der jetzt echt gerade die Kippen im Klo runtergerspühlt hat oder nur so tut, um die selbst zu behalten.
Herr Rüsing zischt ab in das Schulgebäude. Ich ärgere mich ein bisschen wegen dem Anpfiff, aber beschließe schnell abzuhauen. Auch für den Fall, das der wieder rauskommt.
Ich mache den Sprithahn wieder auf und starte schnell das Mofa. Herr Rüsing guckt aus dem ersten Stock zu und schneidet erregt irgendwelche Grimassen.
Bei laufendem Motor scheint das Tropfen nichtmehr so stark zu sein, und ich fahre nur eine geringe Benzinspur hinterlassend gen Sportplatz davon.
Damit ich wieder zu dem hinteren Asphalt Weg komme muss ich wohl ein Stück über die hundert Meter Bahn fahren. Kein Problem denke ich mir und gebe Vollgas, weil der Motor sonst stottert.
Am Ende der Bahn, als ich gerade auf die kleine Strasse seitlich vom Hauptgebäude einbiegen will geht die Kiste wieder aus und es fließt dieses mal mehr Benzin auf den Boden.
Nach kurzem Staunen drehe ich den Benzinhahn zu und Schiebe das Mofa an den Rand. Ich überlege kurz und dann finde ich es aber zu schade um das schöne Benzin ist, wenn das jetzt einfach da so liegen bleibt und auch wegen der Umwelt natürlich, damit das nicht im Boden versickert beschließe ich die Pfütze wieder anzuzünden. Der Rüsing war zum glück nicht praktisch genug gewesen, mir auch das Feuer abzunehmen.
Es gab wieder eine grosse Stichflamme und anschließend ein rußendes Feuerchen. Ich schiebe die Mofa um das Gebäude herum wo sich eine kleine Einbuchtung befindet. Ich nehme einen Schraubenzieher und mache damit am Vergaser ein irgendwelchen Schrauben rum.
Das Benzin scheint auch dort wirklich irgendwie auszutreten. Das Feuer ist schnell wieder runtergebrannt und ich mache einen Test mit dem Ventil und es scheint sich etwas geändert zu haben. Ich packe alles wieder zusammen und will wieder losfahren.
Wenn es jetzt schiebe ich eben nach Hause, denke ich. Das ist von dort aus auch nicht weit. Nach dem starten läuft der Motor aber erstaunlicher Weise relativ stabil. Als ich aber Gas gebe fällt mir direkt auf, das der Motor sofort hochdreht und auch richtig abzieht.
In Gefühlten 2 Sekunden fahre ich 25 km/h und jetzt dreht sie sogar bis auf 33 hoch. Erstaunt komme ich eine Minute Später Zuhause an. Eben das Werkzeug zurück bringen, und dann zum See.
Aus dem Keller schaffe ich es, eine Spühdose mit weissem Lack in einer Jutetasche einzupacken. Keiner im Haus sieht mich, und ich fahre mit der Dose auf dem Gepäckträger richtung Baggerloch.


Anstrenged ist, permanent die Bedingungen erhalten zu müssen unter denen die eigenen Kunst entstehen kann.
Also genug Geld zu verdienen mit irgendwelchen Jobs, Ausstellungen zu organisieren, Atelier Räume zu finden, sichern und unterhalten, 

*/
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  #endif

  #ifdef SENDER1
  memcpy(orgel.peer_addr, Orgel_Address, 6);
  orgel.channel = 13;  
  orgel.encrypt = false;
  if (esp_now_add_peer(&orgel) != ESP_OK){
    Serial.println("Failed to add Orgel");
    return;
  }
  #endif
  
  #ifdef SENDER2
  memcpy(wand.peer_addr, Wand_Address, 6);
  wand.channel = 13;  
  wand.encrypt = false;
  if (esp_now_add_peer(&wand) != ESP_OK){
    Serial.println("Failed to add Wand");
    return;
  }

  #endif
  
  #ifdef SENDER3
  memcpy(board.peer_addr, Board_Address, 6);
  board.channel = 13;  
  board.encrypt = false;
  if (esp_now_add_peer(&board) != ESP_OK){
    Serial.println("Failed to add Board");
    return;
  }

  #endif
 
 

  #ifdef DEBUG
  Serial.print("Setup done...");
  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
  #endif

  if (esp_now_init() != ESP_OK) {
    #ifdef DEBUG
    Serial.println("Error initializing ESP-NOW");
    for(;;); // Don't proceed, loop forever
    #endif
    return;
  }
  
  #ifdef SCREEN
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.drawBitmap((display.width()  - LOGO_WIDTH ) / 2,(display.height() - LOGO_HEIGHT) / 2,logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(500);
  display.clearDisplay();
   
    /*  
    (display.width()  - FLAG_WIDTH ) / 2,
    (display.height() - FLAG_HEIGHT) / 2,
    flag_bmp, FLAG_WIDTH, FLAG_HEIGHT, 1);
    delay(2000);
    display.clearDisplay();
  */
  #endif

  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  

}

#ifdef SCREEN
/*
Screen
Der Bildschirm lässt sich als universelle Bildmaschine betrachten. Er reproduziert optische Informationen durch das Ausstrahlen von Licht.
Licht ist etwas, das viel mit unserer Wahrnehmung macht. Es beeinflusst die Stimmung und unsere Gefühle in nicht geringem Maße.
Durch das Licht des Schirms fühlen wir uns geborgen und sicher, es gibt uns das vertraute Gefühl des Wohnzimmers und der Geborgenheit.



*/



void drawinterface(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2); // Großer Text
  #ifdef SENDER1
  display.println("Orgel");
  #endif
  #ifdef SENDER2
  display.println("Wand");
  #endif
  #ifdef SENDER3
  display.println("Board");
  #endif
  display.setTextSize(1); // kleiner Text
  switch (modus){
    case 1: //direkt steuern
    display.println("Direkt");  
    display.setTextSize(1); 
    display.print("Speedchange: ");
    display.println(speedchange);
    display.print("Speed: ");
    display.println(speed);
    display.print("Steer: ");
    display.println(steer);
    
  
    break;
    case 2: //automatik gerade
    display.println("Gerade");
    display.setTextSize(1); 
    display.print("Laenge: ");
    display.println(distanz);
    display.print("Geschwindigkeit: ");
    display.println(speed);
    display.print("Status: ");
    if (running)display.println("running...");
    if (!running)display.println("Stopped!");
    
    
    
    
    break;
    case 3: //automatik kreis
    display.println("Kreis");
    display.setTextSize(1); 
    display.print("Durchmesser: ");
    display.println(steer);
    display.print("Geschwindigkeit: ");
    display.println(speed);
    display.print("Status: ");
    if (running)display.println("running...");
    if (!running)display.println("Stopped!");
    break;

  }
  
  //display.print(modus[mode]);
  display.print("Battery: ");
  display.println(Feedback.batVoltage);
  display.print("Temp: ");
  display.println(Feedback.boardTemp);
  display.display();
  //display.setCursor(0, 10);
}
#endif

void loop() {
//serialdebug();


#if defined RECEIVER_1 || defined RECEIVER_2 || defined RECEIVER_3

if (Serial.available()) {
    while (Serial.available() && buf_size < BUFFER_SIZE) {
      buf_send[buf_size] = Serial.read();
      send_timeout = micros() + timeout_micros;
      buf_size++;
    }
  }
  #endif
   
  #ifdef RECEIVER_1
  if (buf_size == BUFFER_SIZE_SEND || (buf_size > 0 && micros() >= send_timeout)) {
  esp_err_t result = esp_now_send(Sender1_Address, (uint8_t *) &buf_send, buf_size);
  buf_size = 0;
  }
  #endif
  
  #ifdef RECEIVER_2
  if (buf_size == BUFFER_SIZE_SEND || (buf_size > 0 && micros() >= send_timeout)) {
  esp_err_t result = esp_now_send(Sender2_Address, (uint8_t *) &buf_send, buf_size);
  buf_size = 0;
  }
  #endif

  #ifdef RECEIVER_3
  if (buf_size == BUFFER_SIZE_SEND || (buf_size > 0 && micros() >= send_timeout)) {
  esp_err_t result = esp_now_send(Sender3_Address, (uint8_t *) &buf_send, sizeof(buf_send));
  buf_size = 0;
  }
  #endif
  
  
  
      
#if defined SENDER1 || defined SENDER2 || defined SENDER3
  button1.loop();
  button2.loop();
  button3.loop();
  button4.loop();
  button5.loop();
  int btn1State = button1.getState();
  int btn2State = button2.getState();
  int btn3State = button3.getState();
  int btn4State = button4.getState();
  int btn5State = button5.getState();
  
  readio();
  steerlogic();
  makecommand(steer,speed);
  #endif

  #ifdef SCREEN
  drawinterface();
  #endif

  #ifdef SENDER1
  unsigned long timeNow = millis();
  if (timeNow - prevsend >= TIME_SEND ) { //&& buf_send[0] != 0x00 
  prevsend = timeNow;
  result = esp_now_send(Orgel_Address, (uint8_t *) &Command, sizeof(Command));     
  }
  #endif
  
  #ifdef SENDER2
  unsigned long timeNow = millis();
  if (timeNow - prevsend >= TIME_SEND ) { //&& buf_send[0] != 0x00 
  prevsend = timeNow;
  result = esp_now_send(Wand_Address, (uint8_t *) &Command, sizeof(Command));     
  }
  #endif
  
  #ifdef SENDER3
  unsigned long timeNow = millis();
  if (timeNow - prevsend >= TIME_SEND ) { //&& buf_send[0] != 0x00 
  prevsend = timeNow;
  result = esp_now_send(Board_Address, (uint8_t *) &Command, sizeof(Command));     
  }
  #endif

  #ifdef DEBUG
  serialdebug();
  #endif


}
