# BOSC - Noter

## Kap. 1

## Kap. 2
### Systemkald
 * Funktioner til at anvende operativsystemets devices. 
 * Ses sjældent af udviklere, da mere high-level API'er findes - libc f.eks. 
 * Er indenfor kategorier
 	* process control
 	* file manipulation
 	* device manipulation
 	* information maintenance 
 	* communications 
 	* protection

#### Parameter-passing til systemkald
 * Kan passes via registers. Der kan dog være flere params. end registers.
 * Kan gemmes som block i hukommelse.
 * Kan pushes og poppes på stack af OS og program.

#### System-programmer
 * Simple programmer medfølgende til OS der udfører gængse tasks
 * F.eks. file management, compilere, status information, kommunikation

### Kernel-struktur
 * Simpel kernel - alt er implementeret ét sted. Monolithic.
 	* Hurtigt, simpelt. Lille overhead. 
 	* Svært at vedligeholde.
 * Layered kernel - ting er implementeret i lag. hardware inderst, UI yderst. 
 	* Simpel opbygning. Nemmere at fejlfinde.
 	* Definition af lag og rækkefølge er svær at finde ud af. 
 	* Mindre performance - overhead i komm. mellem lag. 
 * Microkernel - tyndt lag til mest basic ting ml. hardware og user space apps
 	* Nemt at udvide.
 	* (Måske) bedre sikkerhed - alt kører i user space.
 	* Lavere performance - overhead i kommunikation mellem kernel og user space.
 * Loadable kernel modules
 	* Komponenter kan injectes i kerne under kørsel.
 	* Giver fleksibilitet, lille kerne. 
 	* Minder om microkernel, men er uafhængig af implementerede user space apps. 
 Moderne kernels har en kombination af ovenstående.
 Layered er dog ikke så populær. 

## Kap. 3
### Processer
* En process er én arbejdsenhed i OS - f.eks. et program under eksekvering.
* Inkluderer ofte 
	* programkode - text section
	* program counter
	* indhold af process-registre
	* stack - temp. data
	* globale variable
	* evt. heap
* Processer har en given state
	* new
	* running
	* waiting
	* ready
	* terminated
* Processer repræsenteres som **Process Control Blocks (PCB)**, der indeholder
	* Process state
	* Program counter - næste instruktion
	* CPU-registre - pointers osv.
	* CPU-scheduling information - prioritet f.eks. 
	* Memory information - page tables etc.
	* Accounting information - CPU brugt, process numre osv. 
	* I/O status - åbne filer osv.

### Scheduling
* Mange processer kører samtidig og skal styres af en scheduler.
* Alle processer står i job queue. 
* Waiting processer står i ready queue (linked list).
* I/O ventende processer står i device queue.

#### Scheduler Types
* Long term scheduler - læser ventende processer fra disk og putter dem i memory. Styrer mængde af multiprogramming - processer i hukommelse
* Short term scheduler - læser processer fra memory og putter i CPU.
* Medium term scheduler - swapper processer fra mem. til CPU for at øge miks af I/O- og CPU-processer.

#### Interrupts
* Process context gemmes, når interrupt sker (context switch).
* Process kan restores efter interrupt i samme state.

#### Process Lifetimes
* Processer kan starte processer - resulterer i et process-træ.
* Processer tildeles ID (pid).
* Processer kan oprettes på to måder
	* Parent executer parallelt med barn
	* Parent venter på barn bliver færdig
* Processer kan dele adresser med parent på to måder
	* Barn er en klon af forælder
	* Barn erstattes af et nyt program
* Parents kan kun dræbe deres børn - ikke andre processer
* zombie process er når parent ikke kalder wait, men barn er dødt
* orphans er når parent er dødt, men barn er levende

### Interprocess Comunication (IPC)
* Processer kan kommunikere med hinanden via IPC, der kan implementeres på to måder:

#### Shared Memory 
* Processer bliver enige om at dele en del af hukommelsen.
* Ofte implementet vha. buffer
	* Unbounded buffer - producer er ubegrænset, consumer skal måske vente på items
	* Bounded buffer - producer skal vente hvis buffer er fuld, consumer venter hvis buffer er tom 

#### Message Passing
* Processer kommunikerer vha. messages
* Processer requester et communication link mellem dem af OS
* Flere typer kommunikation findes
	* Direct - sender og modtager specifikt til/fra et kendt andet pid
	* Indirect - processer opretholder mailboxes, og henter fra dem
* Message passing kan synkroneres på forskellige måder
	* blocking send - sender er blokeret indtil besked modtages
	* nonblocking send - sender sender og er fortsætter
	* blocking receive - receiver blocks indtil besked ankommer
	* nonblocking receive - receiver henter besked, også selvom den er NULL
* Nonblocking kommunikation foregår ofte med buffers
	* zero capacity - ingen beskeder må vente, sender blocker
	* bounded capacity - n beskeder i kø, sender blocker hvis fuld
	* unbounded capacity - sender blocker aldrig

#### Client Server Kommunikation
* Sockets: To processer bliver enige om at oprette forbindelse der holdes
* Pipes: Har write- og read-end - er anonyme. 
	* Named pipes eksisterer og er åbne for alle fremmede processer.

## Kap. 4
### Threads
* Flere opgaver kan afvikles på én gang med flere tråde. 
* To typer parallelitet findes:
	* Data-parallelitet: data deles på tværs af kerner
	* Task-parallelitet: hver kerne eksekverer unikt task
* Der findes to typer tråde
	* kernel threads
	* user threads

#### Thread models
* Many-to-One: 
	* mange user threads mappes til én kernel thread. 
	* Kernel thread blocker, hvis én user thread blocker. 
	* Giver ikke mulighed for multi-core. 
* One-to-One:
	* Hver user thread mappes til én kernel thread
	* Begrænser mængde tråde mulige, da oprettelse af kernel tråde er dyrt
* Many-to-Many
	* Mange user threads mappes til samme eller mindre antal kernel threads
	* LWP (user tråd gives til process mappet til kernel thread) giver upcall når kernel thread er ved at blocke. Hermed "redder" LWP-process user thread.

### Thread Libraries
* Parent kan lave threads og derefter waite deres arbejde er fuldført - fork-join 
* Pthreads er et spec. til UNIX-tråde. Implementeret af mange OS.
* Windows Threads er specifik Windows-lib.
* Java threads er Java-impl. 

### Implicit Threading
Run-time libs. og compilere kan threade applikationer automatisk - implicit threading

#### Thread Pools
* Antal threads skabes ved process startup. Venter herefter på arbejde.
* Thread returneres til pool når arbejde er færdigt.
* Der kræves ikke ressourcer til thread creation.
* Thread-mangel kan opstå hvis pool er lille.
* Pool kan udvides eller mindskes af OS alt efter arbejde.

#### OpenMP
* Definerer parallelle regioner i kode inden compile. 
* Regioner køres i parallel ved runtime. 

#### Grand Central Dispatch
* Blocks (arbejdsmængde) defineres i kode.
* Blocks puttes i queue og afvikles på tråde når de hentes.
* Implementeret vha. POSIX threads.

### Thread Issues
* `fork()` kan enten kopiere eksisterende tråde eller ej - afhænger af OS.
* Svært at vurdere hvor UNIX signals skal handles med flere tråde.

### Cancellation
* En tråd terminerer anden tråd - asyncronous cancellation.
* Tråd tjekker periodisk om den skal terminere - deferred cancellation.


## Kap. 5
### Critical Sections 
* Race conditions kan undgås ved anvendelse af critical sections i kode.
	* Entry section: request tilladelse til at gå ind i sektion
	* Critical section: sektion imellem hvor én process har adgang ad gangen
	* Exit section: frigiv tilladelse til sektion
* Følgende skal overholdes i en critical section-løsning
	* Mutual exclusion: én process ad gangen.
	* Progress: kun processer der er før deres cricial section må bestemme hvem der får adgang.
	* Bunded waiting: processer får adgang til deres critical section et begrænset antal gange, hvis en anden process venter på adgang.
* To typer kernel-implementationer for critical sections findes
	* preemptive kernel: flere processer får kernel-mode concurrently
	* nonpreemptive kernel: én process kører i kernel-mode indtil den er færdig

### Peterson's solution
* Løsning til critical section-problemet.
* Delt variabel indikerer hvilken process' tur det er.
* Delt bool-array viser hvilke processer der ønsker adgang til section.

### Hardware solutions
* Peterson's solution implementeret vha. hardware-features.
* Locks sker atomically, så én kerne får altid adgang, aldrig to samtidig.
* Hardware locks ikke tilgængelige for developers, men API er ovenpå er tilgængelige.

### Mutex locks
* Mutex er implementation ovenpå hardware locks.
* Implementeret ligesom Peterson's solution.
* Processer har _busy wait_, så de står og looper mens de venter - dårligt.
* Kræver dog ikke context switch, da en proces med det samme får mutex lock.

### Semaphores
* Semaforer tillader at N processer kan være i en critical section samtidig. 
* En process trækker én fra semaphoren, når den går ind i critical section.
* Hvis semaforen er 0, blocker processen.

### Priority inversion
* Hvis en process med højere prioritet venter på en process med lavere prioritet anvendes priority inversion
* Lav-prioritetes-processen får lige so høj prioritet som den ventende proces. 
* Efter arbejde er færdigt, går prioriteten ned igen. Hermed undgås at en mellem-prioritets-proces gør høj-prioritets-processen venter.

## Kap. 6
* Process scheduling: generel scheduling
* Thread scehduling: thread-specifik scheduling
* CPU burst: en process skal bruge CPU'en
* I/O busrt: en process skal bruge en I/O-enhed
* Der eksisterer flere scheduling schemes
	* Nonpreemtive/cooperative: scehduling sker kun hvis process går fra running til waiting state eller bliver færdig med execution. En process har CPU indtil den interrupter eller er done.
	* Preemptive scheduling: Process i ready state bliver også scheduled. Race conditions kan opstå hvis en process wait'er på IO og en anden process for lov til at læse dette data imens. 
* En dispatcher giver CPU til en udvalgt process ved at 
	* skifte context
	* skifte til user mode
	* hoppe til rigtig programinstruktion for at resume processen

### Scheduling criteria
* CPU brug - CPU skal være så travl som muligt.
* Throughput - processer gennem CPU'en over tid.
* Turnaround time - tid fra process starter til den slutter.
* Waiting time - total ventetid.
* Response time - tid fra request til svar.

### Scheduling Algorithms
*  First Come, First Serve
	* Process der requester CPU først får først CPU
	* Implementeret vha. FIFO-kø
	* Simpel
	* Lang ventetid for processer
* Shortest Job First
	* Korteste job gives først CPU
	* Svært at vurdere længde af næste CPU burst for process
	* Preemptive - kortere job skifter nuværende process ud
	* Nonpreemptive - job kører færdig
* Priority Scheduling
	* Processer får CPU efter prioritet
	* Low-priority processer kan "sulte"
	* "Sult" kan undgås ved at prioriteten stiger over tid
* Round Robin 
	* Ready-queue er cirkulær
	* Process får én time slice hver gang den må arbejde
	* Der interruptes efter time slice er udløbet
	* Time slices/quantum kan øges eller mindskes
* Multilevel queue
	* Foreground og background process-grupper
	* Ready queue separeres i individuelle queues
	* Processer deles efter ex. memory size, prioritet etc.
	* Hver queue har egen scheduling algorithm
	* Scheduling mellem queues - f.eks. foreground først
* Multilevel Feedback Queue
	* Processer rykkes til kø efter CPU-brug-karakteristik
	* Processer med høj CPU brug flyttes til lavere prioritet 
	* Udfordrende at finde metode til udvælgelse og flytning af processer

### Thread Scheduling
* Kernel-level threads schedules af OS, user-level manages af thread library.

### Multiple-Processor Scheduling
* Assymetric Multiprocessing - én CPU står for al scheduling
* Symmetric Multiprocessing - hver CPU scheduler sig selv
* Processor affinity - process skal gerne blive på samme CPU så cache-overførsel undgås
	* Soft affinity - process bliver helst på samme CPU, men _kan_ godt flyttes
	* Hard affinity - process kan tvinges til at blive på én CPU

#### Load Balancing
* Arbejde distribueres mellem CPU'er
	* Push migration - processer pushes når en anden CPU er idle
	* Pull migration - processer pulles når en CPU er idle
* Multithreading
	* Coarse-grained multithreading - thread executer indtil ex. memory stall sker. Har dyre skift.
	* Fine-grained multithreading - thread skifter oftere, ex. når instruks er færdig. Har hardware så skift er billigere.

### Realtime CPU Scheduling
* Soft realtime - ingen garanti for hvornår kritisk process køres, har bare højere prioritet.
* Hard realtime - task sker inden sin deadline
* Event latency - tid fra event sendes til det behandles
	* Interrupt latency - tid fra interrupt ankomst til CPU til interrupt-behandling startes. Nuværende state skal gemmes og skiftes.
	* Dispatch latency - tid fra stop af én process til en anden process

#### Priority-Based scheduling
* periodic - processer kræver CPU i intervaller/rates
* deadline - process kræver CPU inden deadline

#### Rate-monotonic scheduling
* Tasks schedules efter prioritet - sat efter periode

#### Earliest Deadline First
* Tidlige deadlines prioriteres højest

### Proportional Share Scheduling
* Time-shares deles ud til processer af fixed number af shares.
* Nye processer afvises hvis der ikke er shares til dem.  

## Kap. 7
Ressourcer tildeles, bruges og friviges af processer. Deadlocks kan opstå.

### Deadlocks
* Deadlocks kan opstå hvis følgende er opfyldt
	* Mutual exclusion - kun én proces har en ressource ad gangen
	* Hold and wait - en process skal holde en ressource og vente på en anden
	* No preemption - en ressource frigives først når en process er færdig med den
	* Circular wait
* Resource-Allocation Graph
	* Graf kan tegnes over processer der bruger ressourcer
	* Edge fra P -> R = process har ressource - request edge
	* Edge fra R -> P = ressource er allokeret til process - assignment edge
	* Hvis en cycle findes er der (måske) deadlock - ingen cycle, ingen deadlock

### Handling Deadlocks
* Deadlocks kan deales med:
	* protokol forhindrer eller undgår deadlocks - avoidance
	* detect deadlocks, recover
	* ignorer deadlocks
* Deadlock prevention
	* En af følgende forhindres ved kontrol af resourcer
		* Mutual Exclusion (nej) - visse ressourcer (locks) kan ikke være excluded 
		* Hold and Wait (nej) - en process kan kun tilgå én proces ad gangen. "Sult", low perfomance.
		* No preemption (nej) - hvis en process ikke kan få alle ressourcer, annullér alt. 
		* Circular wait - Order ressourcer, request dem i rækkefølge. Rækkefølge witness kan anvendes i OS.

### Deadlock avoidance
* process giver besked til OS om hvad den behøver inden execution
* lav perfomance er en risiko

#### Safe State
* ressourcer tildeles kun hvis der findes en safe sequence
	* for alle processer kan enhver process vente, tildeles resourcer og færdiggøre

#### Resource-Allocation-Graph
* Graf opbygges for resource requests inden execution. 
* Hvis cycle opstår tillades request ikke.

#### Banker's Algorithm
* Ny process kommer ind i systemet, erklærer max. resourcer den skal bruge.
* Hvis der er resourcer nok, grantes request, ellers venter processen. 
* Data-strukturer muliggør algoritme
	* Available array - antal ressourcer for resource på index.
	* Max matrix - max demand for proces på resource
	* Allocation matrix - allokerede antal resourcer for process
	* Need - matrix for manglende antal resourcer for proces
* Safety - check om systemet er i safe state
	* Find en ikke-færdig process, der har mindre behov end resourcer tilgængelige
	* Frigør processens brugte ressourcer, sæt processen til at være færdig.
	* Hvis alt lykkes, er state safe.
* Resource-Request
	* En process requester et antal af en resource
	* Processen må ikke requeste flere resourcer end dens max need
	* Hvis processen requester flere resourcer end tilgængeligt, wait
	* Lad system lade som om resourcer allokeres 
	* Hvis state er safe, alloker resourcer

### Deadlock Detection
* System tjekker om deadlock er sket. 
* System prøver at recover fra deadlock hvis den er opstået.
* Inkluderer et vist overhead i detection og recover.

#### Single Instance
* Der eksisterer kun én af hver resource
* Opretter en wait-for graf. Tjekker for cycle

#### Several Instances
* Der eksisterer flere af hver resource
* Data-strukturer muliggør algoritme
	* Available array - antal ledige af hver resource
	* Allocation matrix - antallet af resourcer allokeret til processer
	* Request - antallet af resourcer requestet for en process
* Find ikke-færdig process, der har mindre behov end resourcer tilgængelige
* Alloker resourcer og sæt process til at være done
* Repeat. Hvis en process ikke kan blive færdig, er en deadlock til stede. 

#### Recovery
* Abort process.
	* Abort alle deadlockede processer.
	* Abort én process indtil deadlock cycle er væk.
* Stjæl resourcer fra process. 
	* Svært at vælge hvilken process der stjæles fra
	* Svært at rulle process tilbage, når den ikke kan eksekvere
	* Svært at undgå starvation hvis samme process rammes

## Kap. 8
* Processers memory er separeret fra hinanden for sikkerhed.
* Program for tildelt base-adresse og et limit, der angiver memory for den.
* Programmer står i input queue inden de trækker i memory.
* Kode genereres med relocatable memory adresses. 
* Logical address - memory adress genereret af CPU (virtual address).
* Physical address - reelt register i memory. 
* Relocation register styrer mapping fra logical til physical med offset.
* Static linking compiler libraries med i programkode.
* Dynamic linking trækker relevante instruktioner ind når der er brug for dem.

### Swapping
* Swapping tager en process ud af memory og gemmer den. 
* Processen kan tages ind i memory senere.

### Contiguous Memory Allocation
* To lag memeory - high og low memory.
* OS og interrupt vector i low memory. 
* Transient code kommer og går - loades ind og us af memory som nødvendigt.

### Memory Allocation
* Memory partitions - hver partition indeholder én process.
* Processer puttes i memory-huller fra input queue. 
* First-fit allocation
	* Alloker til første hul der har plads. Lineær søgning enten fra start af memory eller fra sidste location. 
* Best-fit allocation 
	* Alloker det mindste hul der har plads. Lineær søgning, medmindre memory er sorteret. 
* Worst-fit allocation
	* Alloker det største hul der har plads. Lineær søgning, medmindre sorteret. 
* First fit generelt hurtigst. Best fit ikke rigtig bedre end first fit.
* External fragmentation - processer fjernes fra hukommelsen, og skaber nye små huller, der ikke er i forlængelse. 
* 0.5N af N blokke tabes til fragmentation.
* Internal fragmentation - process' ønskede memory er mindre end blokstørrelse.
* Compaction reorder blokke, så memory er i forlængelse. 
	* Kan enten rykke alt brugt til ende - dyrt.
	* Kan tillade hukommelse på tværs af blokke.

#### Segmentation
* Logical address space er en gruppe af segmenter (klumper).
* Segment har navn og længde og offset.
* Logisk adresse er (segment-nummer, offset).
* Logiske adresser skal mappes til fysiske adresser - segment table.
* Segment table har segment base (start) og segment limit (slut).

#### Paging
* Physical memory er delt ind i frames af fixed størrelse.
* Logical memory deles ind i samme størrelse i pages. 
* Addresser har et page number og et page offset. 
* Page og frame size styres af hardware.
* Frame table holder styr på hvilke frames er ledige. 
* Paging øger context switching-tid, da alle adresser altid skal konverteres inden de kan bruges. 
* Page table kan ære gemt i registers - svært ved store page tables.
* Store page tables gemmes i memory. 
* Hardware eksisterer for at gøre page table lookups hurtige. 
* Page table indeholder sikkerheds-bit
	* Page kan være read-write eler read-only
* Page table indeholder valid-bit
	* valid - page er i process' logical address space
	* invalid - page er ikke i process' logical address space

### Structure of the Page Table
* Forward mapped page table - two-level page table
	* Flere page tables inde i page table.
	* logical address har et page number til første page table, og et til andet page table, hvor sidste index endeligt findes. 
	* For lille til 64-bit systemer.
* Hashed page tables
	* hash value er virtual page number
	* har linked list af andre page numbers der hasher til samme adresse
* Inverted Page table	
	* Page table har én entry for hver physical page.
	* Entry har en virtuel adresse og information om process der bruger memory dér

## Kap. 9
* Virtual memory tillader at hele processer ikke skal være i memory.
* Virtual memory giver uniform mængde memery - uafhængig af physical memory.
* Program skal ikke tage stilling til mængde hukommelse
* Flere programmer kan være i memory samtidig
* Giver mindre swapping pga. mere plads

### Demand Paging
* Pages loades som de kræves.
* pager swapper krævede pages ind i memory når de kræves.
* Page fault sker, når en page ikke er loadet i memory - invalid bit kan anvendes.
* Pure demand paging - tag kun pages ind når de kræves.
* Process pauses og resumes ved page fault.
* Effective access time - styres af page fault rate.

### Copy-on-Write
* To processer deler pages, pages markeres som copy-on-write - kopier page hvis den skrives til.
* Pool af frie pages kan anvendes til copies.

### Page Replacement
* Over-allocation memory - flere processer kørende, med max. memory-forbrug end der er hukommelse til.
* Pages skal skiftes ud, når der løbes tør for hukommelse.
* OS finder ubrugt page og skifter ud med krævet page.
* Bit-værdi hvis page skal gemmes til disk - er blevet ændret af process efter læsning. Ellers oerkrives page blot.

#### FIFO Page Replacement
* Ældste page skiftes ud vha. FIFO-queue.
* Perfomance-problem - ældste page kan være ofte brugt globale variable.

#### Least Recently Used
* Page der ikke er blevet brugt længst skiftes ud.
* To mulige implementationer
	* Tid kan associeres med hver page
	* Pages kan lægges i stack - ældste nederst
* Kræver hardware til præcis usage-status 

#### LRU-Approximation
* 8-bit for brug opretholdes for hver 8. time period
* ex. `00000001` er brugt næst-sidst.
* Second-chance page-replacement
	* FIFO-replacement med reference bit
	* Hvis bit er 0 - replace
	* Hvis bit er 1 - giv én chance mere, clear bit
* Enhanced Second-Chance
	* Udvides second-chance med to bit
	* (0,0): ikke brugt eller ændret - bedst at replace 
	* (0,1): ikke brugt, men ændret - skriv og replace
	* (1,0): brugt, men ren - bruges nok igen snart
	* (1,1): brugt og ændret - bruges nok snart, skal skrives til disk
* Counting-based
	* Least frequently used - initial heavy use skiftes aldrig
	* Most frequently used - mindst ofte brugt er ny page

### Page allocation
* Algoritme styrer hvordan X frames deles mellem Y processer.
* Equal allocation
	* Alle processer får lige mange frames
* Proportional allocation 
	* Processer for frames baseret på deres størrelse
* Global vs. local allocation
	* Global allocation - frames "stjæles" fra alle processer
	* Local allocation - frames "stjæles" fra egne frames
* Prepaging - loade krævede pages inden de kræves - f.eks. ved opstart af process

### Thrashing 
* Når en process bruger mere tid på at page fault end at execute
* Èn process faulter, tager fra andre processer, der begynder at faulte.
* Page-Fault Frequency
	* For mange page faults - allokér flere pages til process
	* For få page faults - tag pages fra process


## Kap. 10
* Disks er struktureret som lang array af logical blocks.
* Seek time - tid til disk arm er ved ønsket sector.
* Process sender disk-request til OS
	*  er input/output
	*  hvor disk-adresse er
	*  memory adresse til transfer
	*  sectorer der skal transferres

### Disk Scheduling
* First Come First Serve - ikke hurtigst.
* Shortest Seek Time First - afstand fra disk head, ikke bedst.
* SCAN Scheduling - elevator algorithm, frem og tilbage. 
* Circular SCAN - som SCAN, men cirkulær
* SSFT brugt oftest.

### Disk Management
* Sector har header, data area, trailer. 
* Header og trailer indeholder sector info - error correcting code. 
* ECC regnes ud hver gang der accesses.
* Bootstrap: ROM peger på boot blocks hvor OS starter
* Bad block recovery - fundet bad block redirectes til sund block.
* Swap space kan være en partition eller raw disk. 

## Kap. 11
### Consistency Semantics
* Forskellige filtyper
	* text file
	* source file 
	* executable file
* Fil har navn, ID, type, location, størrelse, protection osv.

#### File operations
* Write pointer - sted hvor næste write skal ske. 
* Read pointer - sted hvor næste read skal ske.
* Open file table - OS har liste over alle åbne filer.

#### Access Methods
* Sequential Access
* Direct Access - fil ses som blokke af data

### Disk Structure
* Directory har styr på alle filer på volume.
* Single-level directory - alle filer i ét lag - probelmatisk, men simpelt
* Two-level directory - hver bruger har eget directory
* Tree-level directory - træstruktur
* Cycles ved links-filer giver problemer ved deletion.
* Mounting giver 

### Sharing
* Sharing af filer har sikkerhedsrisici
* Fil har owner og group.
* Fildelingstjenester er oprettet
	* NIS: gemmer user names, hostnames etc. - usikkert (cleartext)
	* CIFS: active directory holder styr på brugere.
	* LDAP: distribueret naming mechanism - active directory baseret herpå. 
* State opretholdes af parter i fildeling, så man kan recover fra fejl.
* Stateless eksisterer, alt information gemmes i request (NFS).
* Consistency semantics - hvordan brugere henter filer samtidig. 
	* File session - accesses mellem `open()` og `close()`.
	* UNIX Semantics
		* Write i fil er synlig hos andre brugere med det samme.
		* Pointer location kan deles med alle andre.
	* OpenAFS 
		* Write er IKKE synligt med det samme. 
		* Ændringer kun synligt i senere file sessions.

### Protection
* Access types
	* Read
	* Write
	* Execute - ind i memory
	* Append - tilføj information
	* Delete
	* List - list name og attributes
* Access Control List - liste af brugere er har adgang til fil
	* Owner - creator
	* Group - brugere der skal have ens adgang
	* Universe - alle andre
	* Read, write, execute bits er associeret med fil for tre grupper.

## Kap. 12 
* Mange lag giver til sammen adgang til disk
	* I/O control - device driver der snakker med disk controller
	* Basic file system - snakker med I/O control, henter blocks, cacher ofte brugte blocks
	* File organisation module - har styr på filer og deres blokke. 
	* logical file system - har styr på fil-struktur, men ikke data (FCB har styr på ownership etc. for enkelt fil)

### File System Implementation
* Boot control block - hvordan OS bootes fra partitionen.
* Volume control block - block-antal, block-size, free blocks. 
* Directory structure - fil-organisation.
* FCB for filer.
* In-memory mount table - information om volume
* In-memory directory structure - nyligt tilgåede directories
* In memory per-process open-file table - process' nylige directories
* Read/write block buffers - til brug under read/write

#### Virtual File Systems
* Tre-lags arkitektur i file systems
	* Interface - open(), close() etc.
	* Virtual File System - generiske fil-operation-interface
		* inode - individuel fil
		* file - open file
		* superblock - hele filsystem
		* dentry - individuel directory entry 

#### Directory Implementation
* Lineær tabel - dårlig performance, lineær search
* Hash tabel - hurtigere, har dog colissions

#### Allocation methods
* Contiguous Allocation - alle blocks ligger i række
	* Fragmentation er et problem.
* Linked Allocation - blocks fungerer som linked list for fil. 
	* Svært at hoppe til midten af fil.
	* Pointers fylder - kan afhjælpes med clusters, der giver internal fragmentation.
	* Hvis en pointer dør, dør hele filen.
* Indexed Allocation - directory har index block, der indeholder alle pointers til resten af fil. 
	* Pointers fylder en hel/flere blocks.
	* Linked scheme - index block fylder hel disk block, der linker til flere, hvis fil er stor.
	* Multilevel scheme - index block peger på index block, der peger på index block.
	* Combined scheme - index block er på hvor inode (fil-block).

#### Free Space Management
* Frie blocks skal holdes styr på.
* Bit Vector - fylder jo flere blocks der er.
* Linked list - alle frie blocks linkes
* Grouping - hver free block har n fire blocks.
* Counting - block tæller n frie blokke frem.
 