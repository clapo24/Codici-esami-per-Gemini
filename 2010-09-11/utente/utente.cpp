/*
 * Mailbox con monitor
 */

#include <all.h>

const int NMESG = 20;
const int MSG_SIZE = 100;
const int NWRITE = 2;
const int NWMSG = NMESG / NWRITE;
const int NREAD = 4;
const int NRMSG = NMESG / NREAD;


extern natl m1;
struct mailbox_t {
	natl mon;
	bool piena;
	char mess[MSG_SIZE];
} mailbox;


void errore(natq a)
{
	monitor_notifyAll(mailbox.mon);
	printf("Processo errato\n");
	terminate_p();
}

natl finish[NREAD];


void pms(natq a)
{
	char buf[MSG_SIZE];
	for (int i = 0; i < NWMSG; i++) {
		snprintf(buf, MSG_SIZE, "Messaggio numero %lu", i + a*NMESG);
		monitor_enter(mailbox.mon);
		while (mailbox.piena) {
			monitor_wait(mailbox.mon);
			monitor_enter(mailbox.mon);
		}
		mailbox.piena = true;
		memcpy(mailbox.mess, buf, MSG_SIZE);
		monitor_notifyAll(mailbox.mon);
		monitor_leave(mailbox.mon);
	}
	printf("Fine scrittore\n");
	terminate_p();
}

void pml(natq a)
{
	char mess[MSG_SIZE];
	for (int i = 0; i < NRMSG; i++) {
		monitor_enter(mailbox.mon);
		while (!mailbox.piena) {
			monitor_wait(mailbox.mon);
			monitor_enter(mailbox.mon);
		}
		memcpy(mess, mailbox.mess, MSG_SIZE);
		mailbox.piena = false;
		monitor_notifyAll(mailbox.mon);
		monitor_leave(mailbox.mon);
		printf("%s\n", mess);
	}
	printf("Fine lettore\n");
	sem_signal(finish[a]);
	terminate_p();
}

void mio_main(natq a)
{
	mailbox.mon = monitor_ini();
	for (int i = 0; i < NREAD; i++)
		finish[i] = sem_ini(0);
	activate_p(errore, 0, 40, LIV_UTENTE);
	for (int i = 0; i < NWRITE; i++)
		activate_p(pms, i, 20, LIV_UTENTE);
	for (int i = 0; i < NREAD; i++)
		activate_p(pml, i, 20, LIV_UTENTE);
	for (int i = 0; i < NREAD; i++)
		sem_wait(finish[i]);
	pause();

	terminate_p();
}
natl m1;

extern "C" void main()
{
	m1 = activate_p(mio_main, 0, 100, LIV_UTENTE);

	terminate_p();
}
