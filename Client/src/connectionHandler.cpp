#include <connectionHandler.h>
 
using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
 
ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_){}
    
ConnectionHandler::~ConnectionHandler() {
    close();
}
 
bool ConnectionHandler::connect() {
    std::cout << "Starting connect to " 
        << host_ << ":" << port_ << std::endl;
    try {
		tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
		boost::system::error_code error;
		socket_.connect(endpoint, error);
		if (error)
			throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
			tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);			
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
	boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
			tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
		if(error)
			throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, ';');
}

bool ConnectionHandler::sendLine(std::string& line,short opcode) {

    return sendFrameAscii(line, ';',opcode);
}
 
bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    char ch;
    int i = 0;
    short opcode=0;
    char* opcodebytes = new char[2];
    std::string command;
    bool passedfirstspace= false;
    // Stop when we encounter the null character. 
    // Notice that the null character is not appended to the frame string.
    try {
		do{

			getBytes(&ch, 1);

            //only add to frame after opcode decoded
            if (ch==0 && opcode!=0) {
                passedfirstspace = true;
                std::string space = " ";
                frame.append(space);
            }
            else if (i>1 && passedfirstspace) {
                frame.append(1, ch);
            }
            else if (i>=0 && i<2){
                if (i==0){
                    opcodebytes[0]=ch;
                    }
                else{
                    opcodebytes[1]=ch;
                    opcode = bytesToShort(opcodebytes);
                    delete[] opcodebytes;
                    command=findCommandString(opcode);
                    frame.append(command);
                }
            }
            i++;
        }while (delimiter != ch);


    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}
 
bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter,short opcode) {
    char *bytes = new char [2];
    shortToBytes(opcode,bytes);
    bool result= sendBytes(bytes,2);
    delete[] bytes;

    std::string newFrame ;
    int opPosition = frame.find(' ');
    if (opPosition!=-1) {
        result = sendBytes("\0",1);
        newFrame = frame.substr(opPosition + 1);
        result = sendBytes(newFrame.c_str(),newFrame.length());
    }

	if(!result) return false;
	return sendBytes(&delimiter,1);
}
 
// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}



//Encode short to 2 bytes
void ConnectionHandler::shortToBytes(short num, char *bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);

    bytesArr[1] = (num & 0xFF);
}


//Decode 2 bytes to short
short ConnectionHandler::bytesToShort(char* bytesArr) {
    short result = (short)((bytesArr[0] & 0xff) << 8);

    result += (short)(bytesArr[1] & 0xff);

    return result;
}

std::string ConnectionHandler::findCommandString(short opcode) {
    if (opcode==9)
        return "NOTIFICATION";
    else if (opcode == 10)
        return "ACK";
    else
        return "ERROR";
}
