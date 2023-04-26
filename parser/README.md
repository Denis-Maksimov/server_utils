# parser
модуль для обработки http.

## Описание


## состав библиотеки
AST.cpp - структура для абстрактного синтаксического дерева
parser.cpp - десериализация http запроса в AST

bus.cpp - интерфейс взаимодействия между объектами программы по шине


------
MVC.cpp - playground devroom for model/view/controller
test.cpp - examples of coding

## Примеры

### parser:
``` c++
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

```
### bus:

- basic messaging between components
``` c++
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
```
- broadcasting message to all components
``` c++
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
```
- unsubscribing from room
``` c++
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
```
- sending multiple messages

``` c++

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

        //!!! тут баг(или фича) невозмоно всё разом прочитать и передать
        // race condition вроде называется
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //REQUIRE(msgsReceived.size() == 2);
        //REQUIRE(msgsReceived.at(0) == "hello");
        //REQUIRE(msgsReceived.at(1) == "world");
	} 

```

## status

- parser  *raw_ok*

- bus     *raw_ok*

- MVC     *not begined*

