# API Structure

`concrete-core` is a modular library based on two main components:

![core_architecture](../_static/core.png)

- The [`specification`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/index.html)
module contains a blueprint (in the form of Rust Traits) of the FHE scheme exposed
in `concrete-core`.

- The `backends` module contains submodules (such a submodule, we call a _backend_ in this document)
  , which implement all or a part of the specification.

Those backends may rely on different hardware resources, which may not always be available. For this
reason, it is possible to include and exclude backends from a build, by acting on the
associated `backend_*` feature flag.

## Specification

The `specification` module describes two kinds of objects which can be implemented in a backend.

### Entities
All the traits appearing in the
[`specification::entities`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/entities/index.html)
module represent the _datatypes_ manipulated in the library (we call _entities_ all these datatypes
we use in the library). To mention a few of them, we have:

+ [`PlaintextEntity`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/entities/trait.PlaintextEntity.html)
+ [`LweSecretKeyEntity`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/entities/trait.LweSecretKeyEntity.html)
+ [`LweCipertextEntity`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/entities/trait.LweCiphertextEntity.html)
+ and more

Only _one_ of the `*Entity` traits can be implemented at once, by a type exported by a backend. If a
structure implements `PlaintextEntity`, it can not be `LweCiphertextEntity` at the same time, for
instance. More details about the entities can be found [here](memory_management.md).

+ add section about "Vector" entities

### Engines
All the traits appearing in the
[`specification::engines`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/index.html)
module represent the _operators_ which can be used to manipulate entities in the library (we call _
engines_
all these operators we use in the library). For instance, we have:

+ [`LweSecretKeyGenerationEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweSecretKeyGenerationEngine.html)
+ [`LweCiphertextEncryptionEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweCiphertextEncryptionEngine.html)
+ [`LweCiphertextDecryptionEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweCiphertextDescryptionEngine.html)
+ and more

If you read between the lines, the fact that we use traits to represent operators, means that we
will have to use special objects (which we call _engines_ in this document), to perform the
operations. This is slightly different from an object model in which operators are usually tied to
the data themselves. In practice, this makes sense because we tend to use _side-resources_ to
perform operations, and those are easier to handle when stored in a separate structure.

Contrary to entities, there is no restrictions on how many `*Engine` traits a type can implement. In
practice, we tend to expose very few engines in a given backend, except for the default backend.

## Backends

As mentioned earlier, backends contain the implementation of a subset of engines and entities from the specification. The full structure of backends in Concrete-core is represented in the following figure:

![core_backends](../_static/core_backends.png)

Four backends are available: 
+ The default backend: contains Zama's own CPU based implementation of the scheme. It is located at
[`backends::default`](https://docs.rs/concrete-core/1.0.0/concrete_core/backends/default/index.html)
. The associated feature flag is the `backend_default`, but since it is the most prominent backend for
now, we include it automatically (it is part of the `default` flag). It does not contain any hardware specific instructions unless configured otherwise. It is possible to configure it to activate x86_64 specific acceleration for the encryption and creation of keys (with `aesni` and `rdseed` features). It also implements engines that accelerate some operations with multithreading (for now, the bootstrap key creation only). Finally, it also implements engines dedicated to serialization.
+ The FFT backend: this backend implements engines that require an FFT implementation, and relies on an in-house FFT implementation for it. For example, such operations are the bootstrap, the external product and the Cmux. It also implements operations to perform a large precision bootstrap (up to 16 bits) while relying on relatively small polynomial sizes.
+ The Cuda backend: this backend exposes two Cuda accelerated implementations of the bootstrap, as well as a Cuda accelerated implementation of the keyswitch.

In these backends, you will find the same structure as in the `specification`. Let's take the example of the default backend's structure (they all follow the same logic):

+ One [`engines`](https://docs.rs/concrete-core/1.0.0/concrete_core/backends/default/engines/index.html)
module containing the engines exported by the `default` backend

+ One [`entities`](https://docs.rs/concrete-core/1.0.0/concrete_core/backends/default/entities/index.html)
module containing the entities exported by the `default` backend

In the `entities` module, among other types, we find the
[`LweCiphertext64`](https://docs.rs/concrete-core/1.0.0/concrete_core/backends/default/entities/struct.LweCiphertext64.html)
type. It is an _entity_, which implements the
[`LweCiphertextEntity`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/entities/trait.LweCiphertextEntity.html)
trait (this type is actually listed in the implementors of the type).

In the `engines` module, we find three types: 

+ [`DefaultEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/backends/default/engines/struct.DefaultEngine.html)
+ [`DefaultParallelEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/backends/default/engines/default_parallel_engine/struct.DefaultParallelEngine.html)
+ [`DefaultSerializationEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/backends/default/engines/default_serialization_engine/struct.DefaultSerializationEngine.html)

`DefaultEngine` is an _engine_ which implements many `*Engine` traits, among which the
[`LweCiphertextEncryptionEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweCiphertextEncryptionEngine.html)
trait, or the
[`LweSecretKeyGenerationEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweSecretKeyGenerationEngine.html)
trait, both of which are implemented for 32 and 64 bits precisions.
`DefaultParallelEngine` on the other hand implements only a subset of those, relying on multithreading to accelerate the computations (via the `rayon` crate).
This is particularly useful to accelerate the creation of bootstrap keys for example.
Finally, `DefaultSerializationEngine` implements engines dedicated to the serialization of
ciphertexts and keys.

For a more detailed description of how to use the default backend, head to the [next page](backend_default.md).
The FFT and Cuda backends are also described in dedicated pages ([FFT backend](backend_fft.md), [Cuda backend](backend_cuda.md)).

## Operator semantics

As much as possible, we try to support different semantics for each operator
in `specification::engines`.

### Pure operators

They take their inputs as arguments, allocate the objects holding the results, and return them. We
call them _pure_ in the sense of pure functions, because they do not have side effects on entities (
though they may have side effects on the engine). These engine traits do not have any particular
prefixes in their names. When non-pure variants of the operator exist, the pure variant tend to
require more resources, because of the allocations it does. Example of such engine traits include:

+ [`LweCiphertextEncryptionEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweCiphertextEncryptionEngine.html)
+ [`LweBootstrapKeyGenerationEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweBootstrapKeyGenerationEngine.html)

### Discarding operators

They take both their inputs and outputs as arguments. In those operations, the data originally
available in the outputs is not used for the computation. We call them _discarding_ because they
discard the data which exist in the output argument and replace it with something else. The engine
traits following this semantics contain the `Discarding` word in their names. Examples of such
engines traits include:

+ [`LweCiphertextDiscardingAdditionEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweCiphertextDiscardingAdditionEngine.html)
+ [`LweCiphertextDiscardingKeyswitchEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweCiphertextDiscardingKeyswitchEngine.html)

### Fusing operators

They take both their inputs and outputs as arguments. In those operations though, the data
originally contained in the output is used for computation. We call them _fusing_, because they fuse
input arguments into the output argument, which is used in the process (as opposed to _discarded_).
The engine traits which follow this semantics contain the `Fusing` word in their names. Examples of
such engines include:

+ [`LweCiphertextFusingAdditionEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweCiphertextFusingAdditionEngine.html)
+ [`LweCiphertextCleartextFusingMultiplicationEngine`](https://docs.rs/concrete-core/1.0.0/concrete_core/specification/engines/trait.LweCiphertextCleartextFusingMultiplicationEngine.html)

## Error Management

Every `*Engine` trait is tied to a special `*Error` of the same name. For instance,
`LweCiphertextVectorFusingAdditionEngine` is tied to the `LweCiphertextVectorFusingAdditionError`
enum whose definition is:

```rust
pub enum LweCiphertextVectorFusingAdditionError<EngineError: Error> {
    LweDimensionMismatch,
    // General error case
    CiphertextCountMismatch,
    // General error case
    Engine(EngineError),     // Engine-specific error case
}
```

This error is returned by the engine, if an error occurred while performing the operation. The two
first error cases are _general_, in the sense that they can be produced by any engine implementing
`LweCiphertextVectorFusingAdditionEngine`. The last error case is a placeholder for an `EngineError`
type parameter, associated to the engine itself. This `EngineError` will be different for any
engine, and contains error cases specific to a given engine.

## Unchecked operators entry points

Every `*Engine` trait has two entry points:

+ One _safe_ entry point, which returns a `Result<SomeType, SomeError>`
+ One _unsafe_ entry point, which only returns `SomeType`, and is suffixed with `_unchecked`

For instance, here is the declaration of the `LweCiphertextEncryptionEngine`:

```rust
pub trait LweCiphertextEncryptionEngine<SecretKey, Plaintext, Ciphertext>: AbstractEngine where
    SecretKey: LweSecretKeyEntity,
    Plaintext: PlaintextEntity,
    Ciphertext: LweCiphertextEntity, {
    fn encrypt_lwe_ciphertext(
        &mut self,
        key: &SecretKey,
        input: &Plaintext,
        noise: Variance
    ) -> Result<Ciphertext, LweCiphertextEncryptionError<Self::EngineError>>;
    unsafe fn encrypt_lwe_ciphertext_unchecked(
        &mut self,
        key: &SecretKey,
        input: &Plaintext,
        noise: Variance
    ) -> Ciphertext;
}
```

Both of them perform the same operation, but the safe entry points will check that all the
preconditions on which the operator rely are verified, and return a descriptive error if this is not
the case.

We chose to mark the unchecked entry point with the `unsafe` keyword, to bring the attention of the
users on the operator documentation. If you want to use those, you have to take it on you to ensure
that the documented preconditions hold before calling the operator.

That's it for this general introduction to the API. 