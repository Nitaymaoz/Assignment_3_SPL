package bgu.spl.net.api.bidi;

//We will use this class to map a unique ID for each active client connected to the server
//I.E  - A client wants to send a message, we will use a kind of cluster to find the correct client that the message should be sent to
//       and now the connections will know who to send the message to by ID

import bgu.spl.net.srv.bidi.ConnectionHandler;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void broadcast(T msg);

    void disconnect(int connectionId);

    void connect(int connectionId, ConnectionHandler<T> connectionHandler);
}
