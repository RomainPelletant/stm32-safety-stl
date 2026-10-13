ST X-CUBE-STL-U5 integration placeholder
#########################################

Status
******

The provider reserves ``CONFIG_SAFETY_STM32_STL_U5`` for an STM32U5 vendor
adapter. It is gated by the hidden
``CONFIG_SAFETY_STM32_STL_U5_PACKAGE_AVAILABLE`` symbol and cannot currently be
selected.

This directory does not claim that the G0 binary or adapter is compatible with
STM32U5. The provider must remain unavailable until the exact ST package has
been added and the following information has been verified from its release
notes and safety documentation:

* package name and version;
* supported STM32U5 device references and Cortex-M33 configuration;
* binary ABI, floating-point assumptions, and supported toolchains;
* exported scheduler and diagnostic APIs;
* diagnostic identifiers and result mappings;
* RAM backup-buffer size, alignment, and section size;
* Flash reference-data format and placement, if applicable;
* permitted startup and runtime execution phases;
* interrupt masking and other execution constraints;
* applicable standard variant and assumptions of use.

Expected package layout
***********************

When supplied, keep the vendor package separate from the generic framework,
for example:

.. code-block:: text

   vendor/stm32u5/
       Inc/
       Lib/
       Src/
       LICENSE.txt
       Release_Notes.html

The future U5 adapter belongs in this module and may reuse generic adapter
helpers only after ABI equivalence has been established. Once the provider is
implemented and verified, its Kconfig file may set the package-availability
gate to ``y``.
