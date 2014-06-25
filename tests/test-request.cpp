#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include "yahttp/yahttp.hpp"

using namespace boost;


BOOST_AUTO_TEST_SUITE(test_request)

BOOST_AUTO_TEST_CASE(test_request_parse_get)
{
  std::ifstream ifs("request-get-ok.txt");
  YaHTTP::Request req;
  ifs >> req;
  
  BOOST_CHECK_EQUAL(req.method, "GET");
  BOOST_CHECK_EQUAL(req.url.path, "/");
  BOOST_CHECK_EQUAL(req.url.host, "test.org");
}

BOOST_AUTO_TEST_CASE(test_request_parse_get_arl)
{
  std::ifstream ifs("request-get-ok.txt");
  YaHTTP::Request req;
  YaHTTP::AsyncRequestLoader arl;
  arl.initialize(&req);

  while(ifs.good()) {
    char buf[1024];
    ifs.read(buf, 1024);
    if (ifs.gcount()) { // did we actually read anything
      ifs.clear();
      if (arl.feed(std::string(buf, ifs.gcount())) == true) break; // completed
    }
  }
  BOOST_CHECK(arl.ready());

  arl.finalize();

  BOOST_CHECK_EQUAL(req.method, "GET");
  BOOST_CHECK_EQUAL(req.url.path, "/");
  BOOST_CHECK_EQUAL(req.url.host, "test.org");
}

BOOST_AUTO_TEST_CASE(test_request_parse_incomplete_fail) {
  std::ifstream ifs("request-get-incomplete.txt");
  YaHTTP::Request req;
  BOOST_CHECK_THROW(ifs >> req , YaHTTP::ParseError);
}

BOOST_AUTO_TEST_CASE(test_request_parse_post)
{
  std::ifstream ifs("request-post-ok.txt");
  YaHTTP::Request req;
  ifs >> req;

  BOOST_CHECK_EQUAL(req.method, "POST");
  BOOST_CHECK_EQUAL(req.url.path, "/test");
  BOOST_CHECK_EQUAL(req.url.host, "test.org");

  BOOST_CHECK_EQUAL(req.POST()["Hi"], "Moi");
  BOOST_CHECK_EQUAL(req.POST()["M"], "Bää");
  BOOST_CHECK_EQUAL(req.POST()["Bai"], "Kai");
  BOOST_CHECK_EQUAL(req.POST()["Li"], "Ann");
}

BOOST_AUTO_TEST_CASE(test_request_parse_cookies) 
{
  std::ifstream ifs("request-get-cookies-ok.txt");
  YaHTTP::Request req;
  ifs >> req;

  BOOST_CHECK_EQUAL(req.COOKIES()["type"].value, "data");
  BOOST_CHECK_EQUAL(req.COOKIES()["more"].value, "values");
  BOOST_CHECK_EQUAL(req.COOKIES()["just"].value, "like this");
  BOOST_CHECK_EQUAL(req.COOKIES()["even"].value, "more");
  BOOST_CHECK_EQUAL(req.COOKIES()["cookies"].value, "kääkkä");
}

BOOST_AUTO_TEST_CASE(test_request_build_post) 
{
  YaHTTP::Request req;
  req.setup("POST", "http://example.org/test");
  req.POST()["one"] = "w";
  req.preparePost();

  BOOST_CHECK_EQUAL(req.str(),  
"POST /test HTTP/1.1\r\n\
Content-Length: 5\r\n\
Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n\
Host: example.org\r\n\
User-Agent: YaHTTP v1.0\r\n\
\r\n\
one=w"
  );
}

}
