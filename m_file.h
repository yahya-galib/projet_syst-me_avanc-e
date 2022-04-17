#ifndef M_FILE_H
#define M_FILE_H
#include "m_utility.h"


MESSAGE *m_connexion(const char *nom, int options,...);

int m_deconnexion(MESSAGE *file);

int m_destruction(const char *nom);

int m_envoi(MESSAGE *file, const void *msg, size_t len, int msgflag);

ssize_t m_reception(MESSAGE *file, void *msg, size_t len, long type, int flags);

size_t m_message_len(MESSAGE *file);

size_t m_capacite(MESSAGE *file);

size_t m_nb(MESSAGE *file);


#endif