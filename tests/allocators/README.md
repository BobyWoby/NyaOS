# Allocator tests

From the repository root, run:

```sh
./tests/allocators/run.py
```

The runner needs Python 3 and a native C compiler (`cc`, or set `CC` to the
compiler executable). It builds into a temporary directory, runs each case in
a separate process with a five-second timeout, and returns a nonzero status if
any case fails, crashes, or times out. Select individual cases by name:

```sh
./tests/allocators/run.py pfa.boundaries slab.creation
```

The tests exercise the current production C sources, not a rewritten allocator.
PFA tests use a controlled in-memory bitmap. Slab tests replace the frame
allocator and HHDM conversion with a page-aligned host-memory mock, including
failure injection. They therefore do not validate Limine, page tables, actual
physical memory, concurrency, or QEMU boot behavior. Run the normal QEMU boot
separately after allocator changes.

The suite describes the desired allocator behavior. Failures are expected while
the allocator is unfinished; a failure in cache creation prevents later steps
within that particular case, but other cases still run. `slab.hash_growth` checks
the hash implementation separately. `slab.failure` checks failure of the first
backing allocation; further partial-failure cases can be added when the
allocator exposes a cleanup contract. The mock uses real host frees, so invalid
frees can crash the isolated case rather than corrupt the rest of the suite.
