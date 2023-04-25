#include "parser.hpp"
#include "bus.hpp"

// Ниже приведен несколько примеров тестов для классов myBus и myComponent:
#define TEST_CASE(a) std::cout<< a<<std::endl;
#define REQUIRE(expr) do{ if(!(expr)) throw std::runtime_error("test failed:"#expr); }while(0)


void bus_test()
{
	TEST_CASE("Test basic messaging between components")
	{
        myBus bus;
        myComponent client1("cli1");
        myComponent client2("cli2");
        bus.subscribe("Room1", &client1, [&](const std::string& msg){
            REQUIRE(msg == "hello");
        });
        bus.subscribe("Room1", &client2, [&](const std::string& msg){
            REQUIRE(msg == "hello");
        });

        client1.sendMsg(bus,"Room1", "hello");
        client2.sendMsg(bus,"Room1", "hello");
	}

	TEST_CASE("Test broadcasting message to all components")
	{
        myBus bus;
        myComponent client1("cli1");
        myComponent client2("cli2");
        myComponent client3("cli3");
        bus.subscribe_broadcast(&client1, [&](const std::string& msg){
            REQUIRE(msg == "broadcast test");
        });
        bus.subscribe_broadcast(&client2, [&](const std::string& msg){
            REQUIRE(msg == "broadcast test");
        });
        bus.subscribe_broadcast(&client3, [&](const std::string& msg){
            REQUIRE(msg == "broadcast test");
        });
        client1.sendMsg(bus,"::broadcast::", "broadcast test");
	}

	TEST_CASE("Test sending message to non-existent room")
	{
        myBus bus;
        myComponent client1("cli1");
        bus.subscribe("Room1", &client1, [&](const std::string& msg){
            REQUIRE(false); // should not receive message
        });
        // sending message to non-existent room, no components should receive it
        client1.sendMsg(bus,"NonExistentRoom", "hello");
	}

	TEST_CASE("Test sending message from component not subscribed to room")
	{
        myBus bus;
        myComponent client1("cli1");
        myComponent client2("cli2");
        bus.subscribe("Room1", &client1, [&](const std::string& msg){
            REQUIRE(msg == "hello");
        });
        // client2 not subscribed to Room1, should not receive message
        client2.sendMsg(bus,"Room1", "hello");
	}

	TEST_CASE("Test unsubscribing from room")
	{
        myBus bus;
        myComponent client1("cli1");
        myComponent sender("sender");
        sender.sendMsg(bus,"Room1", "hello");
        bus.subscribe("Room1", &client1, [&](const std::string& msg){
            REQUIRE(false); // should not receive messages after unsubscribing
        });
        
        bus.unsubscribe("Room1",&client1);
        // client1.unsubscribe("Room1");
        sender.sendMsg(bus,"Room1", "hello");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	TEST_CASE("Test sending multiple messages")
	{
        myBus bus;
        myComponent client1("cli1");
        myComponent client2("cli2");
        std::vector<std::string> msgsReceived;
        bus.subscribe("Room1", &client1, [&](const std::string& msg)
        {
            msgsReceived.push_back(msg);
        });
        bus.subscribe("Room1", &client2, [&](const std::string& msg)
        {
            msgsReceived.push_back(msg);
        });

        
        client1.sendMsg(bus,"Room1", "hello");
        client2.sendMsg(bus,"Room1", "world");

        //!!!
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        REQUIRE(msgsReceived.size() == 2);
        REQUIRE(msgsReceived.at(0) == "hello");
        REQUIRE(msgsReceived.at(1) == "world");
	} 
}

void usage()
{
    std::string input = "GET /index.html HTTP/1.1\r\n"
                        "Host: www.example.com\r\n"
                        "User-Agent: Mozilla/5.0\r\n"
                        "Accept: */*\r\n"
                        "Content-Type: application/json\r\n"
                        "Content-Length: 23\r\n"
                        "\r\n"
                        "{\"key\":\"value\",\"k2\":\"v2\"}";

    ASTNode httpRequest = parseHTTPRequest(input);
    std::cout << httpRequest.toString() << std::endl;

    bool isValid = isASTValid(httpRequest);
    if (isValid)
    {
        std::cout << "Valid AST" << std::endl;
    } else {
        std::cout << "Invalid AST" << std::endl;
    }

    auto urlNode = httpRequest["headers"];
    if (urlNode)
    {
        std::cout <<urlNode.value().toString()<< std::endl;
    }

}



// void errors()
// {
//     std::string input = "GET /index.html HTTP/1.1\r\n"
//                         "Host: www.example.com\r\n"
//                         "User-Agent: Mozilla/5.0\r\n"
//                         "Accept: */*\r\n"
//                         "Content-Type: application/json\r\n"
//                         "Content-Length: 23\r\n"
//                         "\r\n"
//                         "{\"key\":\"value\",\"k2\":\"v2\"}";

//     ASTNode httpRequest = parseHTTPRequest(input);
//     std::cout << httpRequest.toString() << std::endl;

//     bool isValid = isASTValid(httpRequest);
//     if (isValid)
//     {
//         std::cout << "Valid AST" << std::endl;
//     } else {
//         std::cout << "Invalid AST" << std::endl;
//     }

//     auto urlNode = httpRequest["headers"];
//     if (urlNode)
//     {
//         std::cout <<urlNode.value().toString()<< std::endl;
//     }

// }









int main(int argc, char const *argv[])
{
    usage();
    // errors();
    bus_test();
    return 0;
}
