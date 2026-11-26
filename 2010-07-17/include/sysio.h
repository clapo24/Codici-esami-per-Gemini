/// @file sysio.h
/// @brief Primitive realizzate dal modulo sistema e riservate al modulo I/O.

/**
 * @brief Crea un processo esterno.
 *
 * La primitiva crea un processo esterno e lo associa ad una particolare richiesta
 * di interruzione (IRQ).
 *
 * Il processo eseguirà `f(a)` con priorità `prio` a livello `liv`.
 *
 * Il tipo da associare alla richiesta `irq` è passato indirettamente tramite
 * il parametro `prio`. In particolare, il tipo è ottenuto da `prio` sottraendo
 * @ref MIN_EXT_PRIO.
 *
 * @param f		corpo del processo
 * @param a		parametro per il corpo del processo
 * @param prio		priorità del processo
 * @param liv		livello del processo (LIV_UTENTE o LIV_SISTEMA)
 * @param irq		IRQ gestito dal processo
 * @return		id del nuovo processo, o 0xFFFFFFFF in caso di errore
 */
extern "C" natl activate_pe(void f(natq), natq a, natl prio, natl liv, natb irq);

/**
 * @brief Attende la prossima richiesta di interruzione.
 */
extern "C" void wfi();

/**
 * @brief Traduzione da indirizzo virtuale a fisico.
 *
 * Usa il TRIE del processo corrente.
 *
 * @param ff		indirizzo virtuale da tradurre
 * @return 		indirizzo fisico corrispondente (0 se non mappato)
 */
extern "C" paddr trasforma(void* ff);

/**
 * @brief Accessibile in scrittura.
 *
 * Flag da passare ad access() per controllare che l'intervallo di indirizzi
 * sia accessibile in scrittura.
 */
#define A_WRITABLE	0x1

/**
 * @brief Accessibile da ogni processo.
 *
 * Flag da passare ad access() per controllare che l'intervallo di indirizzi
 * cada interamente nella parte utente/condivisa della memoria virtuale.
 */
#define A_SHARED	0x2

/**
 * @brief Verifica dei problemi di Cavallo di Troia
 *
 * La primitiva controlla che tutti gli indirizzi in [`start`, `start` + `dim`)
 * siano accessibili da livello utente. In particolare, tutti gli indirizzi
 * dell'intervallo devono essere mappati.
 *
 * @param start		base dell'intervallo da controllare
 * @param dim		dimensione in byte dell'intervallo da controllare
 * @param flags		ulteriori requisiti, espressi tramite i flag @ref A_WRITABLE
 * 			e @ref A_SHARED (anche entrambi)
 * @return		true se i vincoli sono rispettati, false altrimenti
 */
extern "C" bool access(const void* start, natq dim, natq flags = 0);

/**
 * @brief Riempi un gate della IDT.
 *
 * Il gate sarà in ogni caso di tipo trap.  Il gate non deve essere già
 * occupato. Il tipo deve essere compreso tra 0x40 e 0x4F (inclusi).
 *
 * @param tipo		tipo del gate da riempire
 * @param f		funzione da associare al gate
 * @return 		false in caso di errore, true altrimenti
 */
extern "C" bool fill_gate(natl tipo, vaddr f);

/**
 * @brief Alloca memoria che sia allineata in memoria fisica.
 *
 * @param size		numero di byte da allocare
 * @param align		allineamento richiesto
 *
 * @return 		puntatore alla memoria allocata,
 * 			nullptr in caso di errore
 */
extern "C" void* phys_alloc(size_t size, std::align_val_t align);

/**
 * @brief Dealloca memoria allocata tramite @ref phys_alloc().
 *
 * @param ptr		puntatore alla memoria da liberare
 */
extern "C" void phys_dealloc(void *ptr);
