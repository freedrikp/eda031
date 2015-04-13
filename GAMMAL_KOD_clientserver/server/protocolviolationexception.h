struct ProtocolViolationException {
  private:
    std::string err_msg;

  public:
    ProtocolViolationException(const std:: string msg) : err_msg(msg) {};
    const std::string what() { return err_msg; };
};