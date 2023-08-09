
#include "HttpServer.h"

#include <mynetlib/Logger.h>
// #include "muduo/net/http/HttpContext.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

// using namespace mynetlib;

// 这个回调函数可以用于处理 HTTP
// 请求的默认情况，当服务器无法找到对应的资源时，可以返回一个 404
// 响应。可以根据实际需求修改这个回调函数，以满足的特定需求。
void defaultHttpCallback(const HttpRequest&, HttpResponse* resp) {
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}

// 这样，在构造 HttpServer 对象时，会初始化 server_ 和 httpCallback_
// 成员，并设置相应的回调函数，用于处理连接和消息事件。 server_ 是一个 TcpServer
// 对象，通过传入的参数进行构造，并将 loop、listenAddr、name 和 option
// 分别传递给 TcpServer 的构造函数 httpCallback_ 是一个函数指针，它默认指向
// defaultHttpCallback 函数，这是一个用于处理 HTTP 请求的默认回调函数。
HttpServer::HttpServer(EventLoop* loop,
                       const InetAddress& listenAddr,
                       const std::string& name,
                       TcpServer::Option option)
    : server_(loop, listenAddr, name, option),
      httpCallback_(defaultHttpCallback) {
    // setConnectionCallback() 函数将 onConnection 函数绑定到当前对象的 this
    // 指针上，它将在连接建立或断开时被调用
    server_.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    // setMessageCallback() 函数将 onMessage 函数绑定到当前对象的 this
    // 指针上，它将在接收到消息时被调用，并传递相应的参数
    server_.setMessageCallback(
        std::bind(&HttpServer::onMessage, this, std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3));
}

void HttpServer::start() {
    // LOG_WARN << "HttpServer[" << server_.name()
    //   << "] starts listening on " << server_.ipPort();
    LOG_INFO("HttpServer %s starts listening on %s", server_.name().c_str(),
             server_.ipPort().c_str());
    server_.start();
}

// 这个函数一般在与客户端建立连接成功后被调用，用于初始化连接的上下文信息，以便后续处理该连接的
// HTTP 请求和响应。
void HttpServer::onConnection(const TcpConnectionPtr& conn) {
    // 判断 conn 的连接状态来确定是否已经建立了连接。如果是已连接状态，则调用
    // setContext() 函数，并传入一个 HttpContext 实例作为上下文参数
    if (conn->connected()) {
        // setContext()
        // 函数用于设置与当前连接关联的上下文信息。在这里，将创建一个新的
        // HttpContext 实例作为上下文，并将其存储在连接对象的上下文中。
        conn->setContext(HttpContext());
    }
}

// 这个函数一般在接收到客户端发送的数据后被调用，用于解析和处理 HTTP 请求。
void HttpServer::onMessage(const TcpConnectionPtr& conn,
                           mynetlib::Buffer* buf,
                           Timestamp receiveTime) {
    // 调用 getMutableContext() 函数从连接对象中获取上下文信息，并使用
    // std::any_cast 将其转换为 HttpContext 类型的指针 context
    HttpContext* context =
        std::any_cast<HttpContext>(conn->getMutableContext());

    // 调用 context 的 parseRequest() 函数来解析请求
    if (!context->parseRequest(buf, receiveTime)) {
        // 调用 context 的 parseRequest() 函数来解析请求
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll()) {
        // 成功解析了完整的请求，则调用 onRequest()
        // 函数来处理该请求，并传递连接对象和解析得到的 HttpRequest 对象。
        onRequest(conn, context->request());
        // 调用 context 的 reset() 函数，重置上下文状态，以准备处理下一个请求
        context->reset();
    }
}

// 这个函数用于处理 HTTP
// 请求，并生成相应的响应发送给客户端。具体的请求处理和响应生成逻辑在
// httpCallback_ 回调函数中实现
void HttpServer::onRequest(const TcpConnectionPtr& conn,
                           const HttpRequest& req) {
    // 从请求头中获取 "Connection" 字段的值，并将其保存在 connection 变量中
    const std::string& connection = req.getHeader("Connection");
    // 根据 "Connection" 字段的值和请求的版本信息来判断是否需要关闭连接。如果
    // "Connection" 字段的值为 "close" 或者请求版本为 HTTP/1.0 且 "Connection"
    // 字段的值不为 "Keep-Alive"，则将 close 标志设置为 true，表示需要关闭连接。
    bool close =
        connection == "close" || (req.getVersion() == HttpRequest::kHttp10 &&
                                  connection != "Keep-Alive");
    // 创建一个 HttpResponse 对象，并传入 close 标志作为构造函数的参数。
    HttpResponse response(close);

    // 调用 httpCallback_ 函数，将请求对象 req 和响应对象 response
    // 作为参数，用于处理 HTTP 请求并生成相应的响应。
    httpCallback_(req, &response);
    mynetlib::Buffer buf;
    // 创建一个 Buffer 对象 buf，并调用 response 的 appendToBuffer()
    // 函数，将响应内容添加到 buf 中
    response.appendToBuffer(&buf);

    // 通过调用连接对象的 send() 函数，将 buf 中的数据发送给客户端。
    conn->send(std::string(buf.peek(), buf.readableBytes()));
    // 如果响应中指定需要关闭连接，则调用连接对象的 shutdown() 函数关闭连接
    if (response.closeConnection()) {
        conn->shutdown();
    }
}
