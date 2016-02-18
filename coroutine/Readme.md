
#### VS 설정
##### coroutine 사용을 위한 설정
* Configuration Properties -> C/C++ -> Code Generation -> Basic Runtime Checks 를 Default로 설정
* Configuration Properties -> C/C++ -> Code Generation -> Security Check 를 Disable
* Configuration Properties -> C/C++ -> Command Line 에 /await 추가

##### boost::future::then 사용을 위한 설정
* Configuration Properties -> C/C++ -> Preprocessor Definitions 에 BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION 추가
