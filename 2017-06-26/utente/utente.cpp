#include <all.h>

const int NMESG = 5;
const int MSG_SIZE = 100;


extern natl m1;
extern natl finish;
struct mailbox_t {
	natl mon;
	bool piena;
	char mess[MSG_SIZE];
} mailbox;


void errore(natq a)
{
	monitor_notify(mailbox.mon);
	printf("Processo errato\n");
	terminate_p();
}


void pms(natq a)
{
	char buf[MSG_SIZE];
	for (int i = 0; i < NMESG; i++) {
		snprintf(buf, MSG_SIZE, "Messaggio numero %d", i);
		monitor_enter(mailbox.mon);
		while (mailbox.piena) {
			monitor_wait(mailbox.mon);
		}
		mailbox.piena = true;
		memcpy(mailbox.mess, buf, MSG_SIZE);
		monitor_notify(mailbox.mon);
		monitor_leave(mailbox.mon);
	}
	printf("Fine scrittore\n");
	terminate_p();
}

void pml(natq a)
{
	char mess[MSG_SIZE];
	for (int i = 0; i < NMESG; i++) {
		monitor_enter(mailbox.mon);
		while (!mailbox.piena) {
			monitor_wait(mailbox.mon);
		}
		memcpy(mess, mailbox.mess, MSG_SIZE);
		mailbox.piena = false;
		monitor_notify(mailbox.mon);
		monitor_leave(mailbox.mon);
		printf("%s\n", mess);
	}
	printf("Fine lettore\n");
	sem_signal(finish);
	terminate_p();
}

void mio_main(natq a)
{
	mailbox.mon = monitor_ini();
	activate_p(errore, 0, 40, LIV_UTENTE);
	activate_p(pms, 0, 20, LIV_UTENTE);
	activate_p(pml, 0, 20, LIV_UTENTE);
	sem_wait(finish);
	pause();

	terminate_p();
}
natl m1;
natl finish;

extern "C" void main()
{
	m1 = activate_p(mio_main, 0, 100, LIV_UTENTE);
	finish = sem_ini(0);

	terminate_p();
}
