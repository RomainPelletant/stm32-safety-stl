ST X-CUBE-STL-G0 adapter
########################

Zephyr module separation
************************

This directory is a self-contained Zephyr module named
``stm32-safety-stl``. Its ``zephyr/module.yml`` declares a dependency on the
vendor-neutral ``safety`` module, so Zephyr rejects a workspace containing this
provider without the framework and processes the framework first.

During in-tree development the framework samples add this directory through
``EXTRA_ZEPHYR_MODULES``. When this directory becomes a separate repository,
add both repositories to the product's West manifest and remove the local
``external_modules`` checkout. No generic framework CMake or Kconfig file
needs to reference this provider.

Example product manifest entries:

.. code-block:: yaml

   projects:
     - name: safety
       url: <safety repository URL>
       revision: <revision>
       path: modules/lib/safety

     - name: stm32-safety-stl
       url: <ST provider repository URL>
       revision: <revision>
       path: modules/lib/stm32-safety-stl

Package identity
****************

* Vendor: STMicroelectronics
* Package: X-CUBE-STL-G0
* STL binary version: 2.0.0
* Target enabled by this integration: STM32G071xx
* Vendor binary toolchain: EWARM 9.20.3, with 32-bit ``wchar_t``
* Vendor references named by the release notes: UM2540, UM2544, and AN5659

The checked-in files match the SHA-256 values recorded in the package release
notes. The applicable ST license is in ``vendor/stm32g0/LICENSE.txt``.

Integration scope
*****************

The adapter exposes the individual CPU TM1, TM1L, TM2, TM3, TM4, TM5, TM6,
TM7, TM7L, and TM8 entry points. It preserves their STL index as
``provider_test_id`` and maps ``STL_TmStatus_t`` without collapsing it into a
boolean.

The RAM entry point ``safety_stm32_stl_ram_run_complete()`` accepts an explicit
product-owned address range and repeats the incremental vendor operation until
the configured cycle terminates. The STL-required 8-word backup buffer is
enabled and retained at the start of a dedicated 128-byte
``backup_buffer_section`` linker output section. The remaining 96 bytes are
padding so the rest of SRAM is exactly divisible by the vendor's 128-byte test
sections. The selected test range must exclude that section, the active stack,
and all other live memory that the product integration cannot safely test.
The adapter deliberately does not infer a testable region from the physical
Devicetree SRAM node.

RAM and Flash configurations can also be wrapped in generic test descriptors
with ``SAFETY_STM32_STL_RAM_TEST_INITIALIZER`` and
``SAFETY_STM32_STL_FLASH_TEST_INITIALIZER``. Those descriptors are executed
through ``safety_execute()``, so execution policy remains an instance-level
integration choice rather than a property of the ST diagnostic.

The Flash entry point ``safety_stm32_stl_flash_run_complete()`` accepts an
explicit inclusive range, checks it against the physical Flash described by
Zephyr Devicetree, and preserves ``STL_PARTIAL_PASSED`` while executing the
incremental vendor operation. X-CUBE-STL-G0 divides Flash into 1024-byte
sections and expects its reference CRC table at the end of physical Flash. The
product build must generate and place that table according to ST's integration
procedure; the adapter does not generate safety reference data at runtime. The
table itself must be excluded from the tested range.

No WCET or maximum interrupt-mask duration is asserted by this adapter.
Consequently the descriptors do not advertise either timing capability and the
generic guarded policy rejects them. The product integration must select an
execution phase and policy based on the ST safety documentation and its own
timing analysis.

The adapter does not serialize diagnostic execution. The application/framework
must ensure that STL calls do not overlap and that the selected execution
context satisfies ST's assumptions of use.
