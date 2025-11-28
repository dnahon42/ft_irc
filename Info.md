socket()
├─ Utilité : Créer un socket TCP/IP
├─ Quand : Au démarrage du serveur
├─ Fichier : Socket.cpp (Socket::create())
└─ Exemple : int fd = socket(AF_INET, SOCK_STREAM, 0);

setsockopt()
├─ Utilité : Configurer options socket (SO_REUSEADDR)
├─ Quand   : Juste après socket()
├─ Exemple : ca permet de relancer le serveur directement apres avoir fait ctrl + C donc sans attendre la fin du TIME_WAIT
├─ Fichier : Socket.cpp (Socket::_setReuseAddr())
└─ Cas d'usage : setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

fcntl()
├─ Utilité : Rendre socket NON-BLOQUANT (O_NONBLOCK)
├─ Quand : Après socket(), avant accept()
├─ Fichier : Socket.cpp / Server.cpp
└─ Exemple : fcntl(fd, F_SETFL, O_NONBLOCK);

bind()
├─ Utilité : Lier le socket à une adresse IP:PORT
├─ Quand : Avant listen()
├─ Fichier : Socket.cpp (Socket::bind())
└─ Exemple : bind(fd, (struct sockaddr*)&addr, sizeof(addr));

listen()
├─ Utilité : Mettre le socket en mode écoute
├─ Quand : Après bind()
├─ Fichier : Socket.cpp (Socket::listen())
└─ Exemple : listen(fd, 10);  // backlog = 10

accept()
├─ Utilité : Accepter nouvelle connexion client 
├─ Quand : Quand poll() détecte POLLIN sur serverFd
├─ Fichier : Server.cpp (Server::_acceptNewClient())
└─ Exemple : int clientFd = accept(serverFd, (struct sockaddr*)&addr, &len);

poll()
├─ Utilité : Surveiller PLUSIEURS fd simultanément
├─ Quand : Dans la boucle principale (Server::run())
├─ Fichier : Server_poll.cpp
└─ Exemple : poll(&pollfds[0], pollfds.size(), -1);

recv()
├─ Utilité : Recevoir données d'un client
├─ Quand : Quand poll() détecte POLLIN sur clientFd
├─ Fichier : Server_io.cpp (Server::_receiveData())
└─ Exemple : ssize_t n = recv(fd, buffer, BUFFER_SIZE, 0);

send()
├─ Utilité : Envoyer données à un client
├─ Quand : Quand poll() détecte POLLOUT sur clientFd
├─ Fichier : Server_io.cpp (Server::_sendData())
└─ Exemple : ssize_t n = send(fd, message.c_str(), message.size(), 0);

close()
├─ Utilité : Fermer un file descriptor
├─ Quand : Déconnexion client ou shutdown serveur
├─ Fichier : Server_clients.cpp (Server::_handleClientDisconnect())
└─ Exemple : close(fd);

htons()
├─ Utilité : Host TO Network Short (port)
├─ Quand : Avant bind() pour le port
├─ Fichier : Socket.cpp
└─ Exemple : addr.sin_port = htons(6667);

htonl()
├─ Utilité : Host TO Network Long (IP)
├─ Quand : Pour INADDR_ANY
├─ Fichier : Socket.cpp
└─ Exemple : addr.sin_addr.s_addr = htonl(INADDR_ANY);

inet_ntop()
├─ Utilité : Convertir binary IP → string (moderne)
├─ Quand : Logs, afficher IP client
├─ Fichier : Utils.cpp (NetworkUtils::getIpAddress())
└─ Exemple : inet_ntop(AF_INET, &addr.sin_addr, buffer, INET_ADDRSTRLEN);