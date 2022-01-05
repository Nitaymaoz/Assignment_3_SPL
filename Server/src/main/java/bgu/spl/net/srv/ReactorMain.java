package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoderImpl;
import bgu.spl.net.api.bidi.BidiMessagingProtocolImpl;

public class ReactorMain {
    public static void main(String[] args) {

        try (Server<String> reactor = Server.reactor(4, 1007,
                () -> new BidiMessagingProtocolImpl(new Database()), MessageEncoderDecoderImpl::new);) {
            reactor.serve();
        }
        catch (
                Exception e) {
            e.printStackTrace();
        }

    }
}
