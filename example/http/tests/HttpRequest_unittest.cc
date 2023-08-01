#include "muduo/net/Buffer.h"
#include "muduo/net/http/HttpContext.h"
// 包含了需要的头文件 "muduo/net/Buffer.h" 和 "muduo/net/http/HttpContext.h"

// #define BOOST_TEST_MODULE BufferTest
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using muduo::string;
using muduo::Timestamp;
using muduo::net::Buffer;
using muduo::net::HttpContext;
using muduo::net::HttpRequest;

// 使用Boost.Test 库对 muduo/net/http/HttpContext 类进行单元测试。
// 使用 BOOST_AUTO_TEST_CASE 宏来定义测试用例函数，并使用 BOOST_CHECK 和 BOOST_CHECK_EQUAL 断言宏来进行断言判断。

// 定义了一个测试用例函数 testParseRequestAllInOne，用于测试一次性解析完整的
// HTTP 请求
BOOST_AUTO_TEST_CASE(testParseRequestAllInOne) {
    // 一个 HttpContext 对象
    HttpContext context;
    // 一个 Buffer 对象 input，并向 input 中追加了一个完整的 HTTP 请求报文
    Buffer input;
    input.append(
        "GET /index.html HTTP/1.1\r\n"
        "Host: www.chenshuo.com\r\n"
        "\r\n");

    // 断言宏条件为真，则测试继续进行；如果条件为假，则测试失败并输出相应的错误信息
    BOOST_CHECK(context.parseRequest(&input, Timestamp::now()));
    BOOST_CHECK(context.gotAll());
    // 通过 context.request() 获取解析后的请求对象
    const HttpRequest& request = context.request();
    // 对其进行各种断言判断，例如请求方法、路径、HTTP 版本和头部字段的值。
    BOOST_CHECK_EQUAL(request.method(), HttpRequest::kGet);
    BOOST_CHECK_EQUAL(request.path(), string("/index.html"));
    BOOST_CHECK_EQUAL(request.getVersion(), HttpRequest::kHttp11);
    BOOST_CHECK_EQUAL(request.getHeader("Host"), string("www.chenshuo.com"));
    BOOST_CHECK_EQUAL(request.getHeader("User-Agent"), string(""));
}

// 还定义了测试用例函数 testParseRequestInTwoPieces 和
// testParseRequestEmptyHeaderValue
// 分别用于测试将请求报文拆分成两个片段进行解析和测试空头部字段值的情况。这些测试用例主要验证了
// HttpContext 的解析功能是否正确。



BOOST_AUTO_TEST_CASE(testParseRequestInTwoPieces) {
    string all(
        "GET /index.html HTTP/1.1\r\n"
        "Host: www.chenshuo.com\r\n"
        "\r\n");

    for (size_t sz1 = 0; sz1 < all.size(); ++sz1) {
        HttpContext context;
        Buffer input;
        input.append(all.c_str(), sz1);
        BOOST_CHECK(context.parseRequest(&input, Timestamp::now()));
        BOOST_CHECK(!context.gotAll());

        size_t sz2 = all.size() - sz1;
        input.append(all.c_str() + sz1, sz2);
        BOOST_CHECK(context.parseRequest(&input, Timestamp::now()));
        BOOST_CHECK(context.gotAll());
        const HttpRequest& request = context.request();
        BOOST_CHECK_EQUAL(request.method(), HttpRequest::kGet);
        BOOST_CHECK_EQUAL(request.path(), string("/index.html"));
        BOOST_CHECK_EQUAL(request.getVersion(), HttpRequest::kHttp11);
        BOOST_CHECK_EQUAL(request.getHeader("Host"),
                          string("www.chenshuo.com"));
        BOOST_CHECK_EQUAL(request.getHeader("User-Agent"), string(""));
    }
}

BOOST_AUTO_TEST_CASE(testParseRequestEmptyHeaderValue) {
    HttpContext context;
    Buffer input;
    input.append(
        "GET /index.html HTTP/1.1\r\n"
        "Host: www.chenshuo.com\r\n"
        "User-Agent:\r\n"
        "Accept-Encoding: \r\n"
        "\r\n");

    BOOST_CHECK(context.parseRequest(&input, Timestamp::now()));
    BOOST_CHECK(context.gotAll());
    const HttpRequest& request = context.request();
    BOOST_CHECK_EQUAL(request.method(), HttpRequest::kGet);
    BOOST_CHECK_EQUAL(request.path(), string("/index.html"));
    BOOST_CHECK_EQUAL(request.getVersion(), HttpRequest::kHttp11);
    BOOST_CHECK_EQUAL(request.getHeader("Host"), string("www.chenshuo.com"));
    BOOST_CHECK_EQUAL(request.getHeader("User-Agent"), string(""));
    BOOST_CHECK_EQUAL(request.getHeader("Accept-Encoding"), string(""));
}
