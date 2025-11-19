socket()        ⭐⭐⭐⭐⭐ OBLIGATOIRE
├─ Utilité : Créer un socket TCP/IP
├─ Quand : Au démarrage du serveur
├─ Fichier : Socket.cpp (Socket::create())
└─ Exemple : int fd = socket(AF_INET, SOCK_STREAM, 0);

setsockopt()    ⭐⭐⭐⭐⭐ OBLIGATOIRE
├─ Utilité : Configurer options socket (SO_REUSEADDR)
├─ Quand : Juste après socket()
├─ Fichier : Socket.cpp (Socket::_setReuseAddr())
└─ Exemple : setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

fcntl()         ⭐⭐⭐⭐⭐ OBLIGATOIRE
├─ Utilité : Rendre socket NON-BLOQUANT (O_NONBLOCK)
├─ Quand : Après socket(), avant accept()
├─ Fichier : Socket.cpp / Server.cpp
└─ Exemple : fcntl(fd, F_SETFL, O_NONBLOCK);

bind()          ⭐⭐⭐⭐⭐ OBLIGATOIRE
├─ Utilité : Lier le socket à une adresse IP:PORT
├─ Quand : Avant listen()
├─ Fichier : Socket.cpp (Socket::bind())
└─ Exemple : bind(fd, (struct sockaddr*)&addr, sizeof(addr));

listen()        ⭐⭐⭐⭐⭐ OBLIGATOIRE
├─ Utilité : Mettre le socket en mode écoute
├─ Quand : Après bind()
├─ Fichier : Socket.cpp (Socket::listen())
└─ Exemple : listen(fd, 10);  // backlog = 10

accept()        ⭐⭐⭐⭐⭐ OBLIGATOIRE
├─ Utilité : Accepter nouvelle connexion client
├─ Quand : Quand poll() détecte POLLIN sur serverFd
├─ Fichier : Server.cpp (Server::_acceptNewClient())
└─ Exemple : int clientFd = accept(serverFd, (struct sockaddr*)&addr, &len);

poll()          ⭐⭐⭐⭐⭐ OBLIGATOIRE (ou select/kqueue/epoll)
├─ Utilité : Surveiller PLUSIEURS fd simultanément
├─ Quand : Dans la boucle principale (Server::run())
├─ Fichier : Server_poll.cpp
└─ Exemple : poll(&pollfds[0], pollfds.size(), -1);

⚠️ UN SEUL poll() pour TOUT (serverFd + tous les clientFd)

recv()          ⭐⭐⭐⭐⭐ OBLIGATOIRE
├─ Utilité : Recevoir données d'un client
├─ Quand : Quand poll() détecte POLLIN sur clientFd
├─ Fichier : Server_io.cpp (Server::_receiveData())
└─ Exemple : ssize_t n = recv(fd, buffer, BUFFER_SIZE, 0);

send()          ⭐⭐⭐⭐⭐ OBLIGATOIRE
├─ Utilité : Envoyer données à un client
├─ Quand : Quand poll() détecte POLLOUT sur clientFd
├─ Fichier : Server_io.cpp (Server::_sendData())
└─ Exemple : ssize_t n = send(fd, message.c_str(), message.size(), 0);

close()         ⭐⭐⭐⭐⭐ OBLIGATOIRE
├─ Utilité : Fermer un file descriptor
├─ Quand : Déconnexion client ou shutdown serveur
├─ Fichier : Server_clients.cpp (Server::_handleClientDisconnect())
└─ Exemple : close(fd);

getsockname()   ⭐⭐⭐ UTILE
├─ Utilité : Récupérer adresse IP locale du socket
├─ Quand : Debug, logs
├─ Fichier : Utils.cpp (NetworkUtils::getLocalAddress())
└─ Exemple : getsockname(fd, (struct sockaddr*)&addr, &len);

htons()         ⭐⭐⭐⭐ IMPORTANT
├─ Utilité : Host TO Network Short (port)
├─ Quand : Avant bind() pour le port
├─ Fichier : Socket.cpp
└─ Exemple : addr.sin_port = htons(6667);

ntohs()         ⭐⭐ RARE
├─ Utilité : Network TO Host Short
├─ Quand : Quand tu récupères un port réseau
└─ Exemple : int port = ntohs(addr.sin_port);

htonl()         ⭐⭐⭐ UTILE
├─ Utilité : Host TO Network Long (IP)
├─ Quand : Pour INADDR_ANY
├─ Fichier : Socket.cpp
└─ Exemple : addr.sin_addr.s_addr = htonl(INADDR_ANY);

ntohl()         ⭐⭐ RARE
├─ Utilité : Network TO Host Long
└─ Quand : Lecture adresse IP réseau

inet_addr()     ⭐⭐⭐ UTILE
├─ Utilité : Convertir string IP → binary
├─ Quand : Si tu veux bind sur IP spécifique
└─ Exemple : addr.sin_addr.s_addr = inet_addr("127.0.0.1");

inet_ntoa()     ⭐⭐ RARE (déprécié, utilise inet_ntop)
├─ Utilité : Convertir binary IP → string
└─ Exemple : char* ip = inet_ntoa(addr.sin_addr);

inet_ntop()     ⭐⭐⭐⭐ RECOMMANDÉ
├─ Utilité : Convertir binary IP → string (moderne)
├─ Quand : Logs, afficher IP client
├─ Fichier : Utils.cpp (NetworkUtils::getIpAddress())
└─ Exemple : inet_ntop(AF_INET, &addr.sin_addr, buffer, INET_ADDRSTRLEN);

getprotobyname()    ⭐ PROBABLEMENT INUTILE
├─ Utilité : Récupérer numéro protocole (TCP = 6)
├─ Quand : Rarement nécessaire (tu utilises SOCK_STREAM direct)
└─ Note : Tu peux hardcoder IPPROTO_TCP

gethostbyname()     ⭐ PROBABLEMENT INUTILE (déprécié)
├─ Utilité : Résoudre hostname → IP
├─ Quand : Si tu veux résoudre "localhost" → 127.0.0.1
└─ Note : Déprécié, utilise getaddrinfo() à la place

getaddrinfo()       ⭐⭐ UTILE SI résolution DNS
├─ Utilité : Résoudre hostname/service → adresse
├─ Quand : Si sujet demande support hostname (rare)
├─ Fichier : Socket.cpp (si nécessaire)
└─ Exemple : getaddrinfo("localhost", "6667", &hints, &result);

freeaddrinfo()      ⭐⭐ SI tu utilises getaddrinfo()
├─ Utilité : Libérer mémoire de getaddrinfo()
└─ Exemple : freeaddrinfo(result);

lseek()         ❌ PAS POUR NETWORK
├─ Utilité : Déplacer curseur dans un fichier
└─ Quand : Manipulation fichiers (bonus file transfer ?)

fstat()         ❌ PAS POUR NETWORK
├─ Utilité : Infos sur un fichier
└─ Quand : Bonus file transfer

signal()        ⭐⭐⭐ UTILE pour Server
├─ Utilité : Gérer SIGINT (Ctrl+C), SIGTERM
├─ Quand : Pour shutdown propre du serveur
├─ Fichier : main.cpp ou Server.cpp
└─ Exemple : signal(SIGINT, signalHandler);

sigaction()     ⭐⭐⭐⭐ RECOMMANDÉ (+ robuste)
├─ Utilité : Gérer signaux (mieux que signal())
├─ Quand : Même usage que signal()
└─ Exemple : sigaction(SIGINT, &sa, NULL);

sigemptyset()   ⭐⭐ SI tu utilises sigaction
├─ Utilité : Initialiser set de signaux vide
└─ Exemple : sigemptyset(&sa.sa_mask);

sigfillset()    ⭐ RARE
├─ Utilité : Initialiser set avec TOUS les signaux
└─ Rarement utilisé

sigaddset()     ⭐⭐ SI sigaction
├─ Utilité : Ajouter signal à un set
└─ Exemple : sigaddset(&sa.sa_mask, SIGTERM);

sigdelset()     ⭐ RARE
├─ Utilité : Retirer signal d'un set

sigismember()   ⭐ RARE
├─ Utilité : Vérifier si signal dans set



📋 Résumé par Priorité pour NETWORK
⭐⭐⭐⭐⭐ ABSOLUMENT OBLIGATOIRES (10 fonctions)

1.  socket()         → Créer socket
2.  setsockopt()     → SO_REUSEADDR
3.  fcntl()          → O_NONBLOCK
4.  bind()           → Lier IP:PORT
5.  listen()         → Mode écoute
6.  accept()         → Accepter clients
7.  poll()           → Multiplexing I/O
8.  recv()           → Recevoir données
9.  send()           → Envoyer données
10. close()          → Fermer connexions
11. htons()          → Conversion port
12. inet_ntop()      → IP client (logs)
13. sigaction()      → Shutdown propre (Ctrl+C)
14. htonl()          → INADDR_ANY
15. getsockname()    → Debug/logs
16. getaddrinfo()    → SI résolution DNS
17. freeaddrinfo()   → SI getaddrinfo()
18. ntohs()          → Rare
19. ntohl()          → Rare
20. inet_addr()      → Remplacé par inet_ntop()
21. inet_ntoa()      → Déprécié
22. getprotobyname() → Rarement utilisé
23. gethostbyname()  → Déprécié
24. signal()         → Utilise sigaction() à la place
25. sigemptyset/fillset/addset/delset/ismember → Si sigaction
26. connect()        → Client IRC (INTERDIT)
27. lseek()          → Fichiers (bonus DCC ?)
28. fstat()          → Fichiers (bonus DCC ?)


🎯 Map des Fonctions → Fichiers
Socket.cpp
Code
socket()
setsockopt()
fcntl()
bind()
listen()
accept()
close()
htons()
htonl()
inet_addr() (optionnel)

Server_poll.cpp
Code
poll()           → Boucle principale

Server_io.cpp
Code
recv()
send()

Utils.cpp (NetworkUtils)
Code
fcntl()          → setNonBlocking()
getsockname()    → getLocalAddress()
inet_ntop()      → getIpAddress()

main.cpp
Code
signal() ou sigaction()
sigemptyset()
sigaddset()

onctions que TU vas coder :

 socket() - Créer socket serveur
 setsockopt() - SO_REUSEADDR
 fcntl() - O_NONBLOCK sur tous les fd
 bind() - Lier au port
 listen() - Mode écoute
 accept() - Accepter nouvelles connexions
 poll() - UN SEUL pour tous les fd
 recv() - Recevoir données clients
 send() - Envoyer données clients
 close() - Fermer fd
 htons() - Convertir port
 inet_ntop() - IP client pour logs
 sigaction() - Gérer Ctrl+C
Total : ~13 fonctions pour un serveur IRC complet !