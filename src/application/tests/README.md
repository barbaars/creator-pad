# Test




From https://interrupt.memfault.com/blog/unit-testing-basics


- **Fakes** are a working implementation, but will usually substitute their dependencies with something simpler and easier for a test environment. Example: an in-memory key/value store vs a NOR Flash backed Key/Value store.
- **Stubs** are a trivial implementation that returns canned values, generally always returning valid or invalid values.
- **Mocks** are an implementation that is controlled by the unit test. They can be pre-programmed with return values, check values of arguments, and help verify that functions are called.
