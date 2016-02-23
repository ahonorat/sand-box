template <typename T>
class Templated {
public:
  T t;
};

int main() {
  Templated * a;
  // not possible because Templated isn't a type
  // solution : inherits from a base class not templated
  return 0;
}
