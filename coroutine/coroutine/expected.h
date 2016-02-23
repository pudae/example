#pragma once

#include <experimental/resumable>
#include <boost/optional.hpp>

namespace myco {
namespace detail {

template <typename T>
class expected
{
public:
  expected()
  {
  }

  expected(std::exception_ptr ex) : ex_(ex) {
  }

  expected(T value) : value_(std::move(value)) {
  }

  const T& get() const {
    if (ex_)
      std::rethrow_exception(ex_);
    return *value_;
  }

  T& get() {
    if (ex_)
      std::rethrow_exception(ex_);
    return *value_;
  }

  template <typename U>
  void set_value(U value) {
    value_ = std::forward<U>(value);
  }

  void set_exception(std::exception_ptr ex) {
    ex_ = std::move(ex);
  }

private:
  boost::optional<T> value_;
  std::exception_ptr ex_;
};

template <>
class expected<void>
{
public:
  expected()
  {
  }

  expected(std::exception_ptr ex) : ex_(ex) {
  }

  void get() {
    if (ex_)
      std::rethrow_exception(ex_);
  }

  void set_value() {
  }

  void set_exception(std::exception_ptr ex) {
    ex_ = std::move(ex);
  }

private:
  bool is_initialized_ = false;
  std::exception_ptr ex_;
};

} // namespace detail

template <typename T>
class expected
{
public:
  expected()
    : impl_(std::make_shared<detail::expected<T>>()) {
  }

  expected(std::exception_ptr ex)
    : impl_(std::make_shared<detail::expected<T>>(ex)) {
  }

  expected(T value)
    : impl_(std::make_shared<detail::expected<T>>(std::move(value))) {
  }

  /// TODO : SFINAE
  const T& get() const {
    return impl_->get();
  }

  T& get() {
    return impl_->get();
  }

  template <typename U>
  void set_value(U value) {
    impl_->set_value(std::forward<U>(value));
  }

  void set_exception(std::exception_ptr ex) {
    impl_->set_exception(std::move(ex));
  }

private:
  std::shared_ptr<detail::expected<T>> impl_;
};

template <>
class expected<void>
{
public:
  expected()
    : impl_(std::make_shared<detail::expected<void>>()) {
  }

  expected(std::exception_ptr ex)
    : impl_(std::make_shared<detail::expected<void>>(ex)) {
  }

  void get() {
    return impl_->get();
  }

  void set_value() {
    impl_->set_value();
  }

  void set_exception(std::exception_ptr ex) {
    impl_->set_exception(std::move(ex));
  }

private:
  std::shared_ptr<detail::expected<void>> impl_;
};

} // namespace myco

namespace std {
namespace experimental {

template<class T, class... Args>
struct coroutine_traits<myco::expected<T>, Args...>
{
	struct promise_type
	{
    myco::expected<T> expected;

    promise_type() {
    }

		myco::expected<T> get_return_object()
		{
      return expected;
		}

		bool initial_suspend() const
		{
			return false;
		}

		bool final_suspend() const
		{
			return false;
		}

		template<class U>
		void return_value(U&& value)
		{
			expected.set_value(std::forward<U>(value));
		}

		void set_exception(std::exception_ptr ex)
		{
			expected.set_exception(std::move(ex));
		}
	};
};

template<class... Args>
struct coroutine_traits<myco::expected<void>, Args...>
{
	struct promise_type
	{
		myco::expected<void> expected;

    promise_type() {
    }

		std::shared_ptr<myco::expected<void>> get_return_object()
		{
      return expected;
		}

		bool initial_suspend() const
		{
			return false;
		}

		bool final_suspend() const
		{
			return false;
		}

		void return_value()
		{
			expected.set_value();
		}

		void set_exception(std::exception_ptr ex)
		{
			expected.set_exception(std::move(ex));
		}
	};
};

}	// namespace experimental
} // namespace std

namespace myco {

template<class T>
bool await_ready(expected<T>& exp)
{
  return true;
}

template<class T>
bool await_suspend(expected<T>& exp, std::experimental::coroutine_handle<> resume)
{
  return true;
}

template<class T>
auto await_resume(expected<T>& exp)
{
  return exp.get();
}

} // namespace myco
