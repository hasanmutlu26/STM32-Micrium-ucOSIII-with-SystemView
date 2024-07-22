# __STM32 Micrium ucOS-3 with SystemView__
 This project is the integration of Micrium's uC-OS-III Real-Time Operating System and SEGGER SystemView analyzer tool into STM32 microcontroller. 
  
---

#### Board used in the project: __[Nucleo-F439ZI](https://www.st.com/en/evaluation-tools/nucleo-f439zi.html)__
#### Development Environment: __[STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)__

---

### __Sources__
#### Micrium Sources:
+ uCOS-III Book: __[µC/OS-III: The Real-Time Kernel for the ST Microelectronics STM32](https://www.weston-embedded.com/micrium-books)__
+ uCOS-III Source Code:
  - __[uC-OS3](https://github.com/weston-embedded/uC-OS3)__
  - __[uC-CPU](https://github.com/weston-embedded/uC-CPU)__
  - __[uC-LIB](https://github.com/weston-embedded/uC-LIB)__
+ Example Project for BSP code: __[Micrium_STM3240G-EVAL_OS3](https://www.weston-embedded.com/micrium-examples/category/240-stm3240g-eval)__
#### SystemView Sources:
+ SystemView Download: __[SystemView](https://www.segger.com/downloads/systemview/)__
+ User Manual: __[SystemView User Manual (UM08027)](https://www.segger.com/downloads/systemview/)__
+ SystemView Source Code: __[SystemView, Target Sources
](https://www.segger.com/downloads/systemview/)__
+ STLinkReflash : __[ST-LINK On-Board](https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/)__
  - Make sure you read the instructions carefully, have ST-LINK USB drivers and J-Link software package installed.

---

## __Integration Steps__

The integration is committed into the repository step by step, with instructions and explanations given in each commit. While reading these steps, you can review the related commit alongside.


### Step 1: Creating Project
Related commits: [Default Project Created](https://github.com/hasanmutlu26/STM32-Micrium-ucOSIII-with-SystemView/commit/501837018e039962b92dfd8289465db123dc21f5) & [Pinouts Cleared](https://github.com/hasanmutlu26/STM32-Micrium-ucOSIII-with-SystemView/commit/09736fcb8ee578fb638b3847f166dc45fd49262f)
- A new project is created in STM32 CubeIDE.
- Peripheral and clock configurations are made according to the application.

<br/>

### Step 2: Adding Micrium Source Code to Project
Related commit: [Micrium Source Code Added](https://github.com/hasanmutlu26/STM32-Micrium-ucOSIII-with-SystemView/commit/18bc317e2e8c990fed6d6fabe1de50ccd88b40df)
- The file structure followed is different than the recommended structure in the book.
- A folder named Micrium is created. Three new folders are created for each three source code repositories.
- Source codes from uC-CPU, uC-LIB and uC-OS3 are copied to their respective folders. uC-OS3 source code is inside the "Source" directory.
- There are CPU-specific port codes in each repositories. The appropriate port codes from each source are selected copied into their respective folders. 
- There are configuration header file templates in each three repositories. These files are placed in folders called "Cfg" inside the source repositories. A seperate folder called "Cfg" is created inside the Micrium folder and these configuration files are copied into it. 
- All of the new source code paths are added to source and include paths in project settings.


<br/>

### Step 3: Completing Micrium Integration
Related commit: [Micrium Integration Completed](https://github.com/hasanmutlu26/STM32-Micrium-ucOSIII-with-SystemView/commit/1186483f9f7bd128652d3b368673ff97767703ea)

- First of all, the _dbg_uCOS-III.c file is deleted from Micrium/uC-OS3/Source
- Configurations are made in cpu_cfg and lib_cfg files, according to both user application and tracing needs. Timestamping has to be enabled for tracing.
    + CPU_CFG_NVIC_PRIO_BITS configuration constant must be configured according to the reference manual. It determines number of bits used by the microcontroller for interrupt priorities. It is defined in "Nested vectored interrupt controller" chapter in reference manual. For this project, it needs to be 4. 
- Timebase source has to be changed to TIMx. In default, it is selected as SysTick when the project is first created. It is changed to TIM1.

#### Step 3.1: Including BSP
- When timestamping and interrupt disabled time measurements are enabled, some user defined BSP code is needed. More specifically, CPU_TS_TmrInit() function.
- According to Micrium book, "It is a function that uC-CPU expects to have in order to initialize the timer that will be used for timestamps."
    + The official examples in the Weston-Embedded website and BSP codes in them are investigated. 
    + In the STM3240G-EVAL example project, CPU_TS_TmrInit() function is found in cpu_bsp.c file. Entire file is copied into a new BSP folder.
    + BSP_CPU_ClkFreq() function is also needed. It is found in bsp.c file and copied into the cpu_bsp.c. It is a function that simply returns the SystemCoreClock value.
    + HAL library header is included in cpu_bsp.c
    + Source of example project: https://www.weston-embedded.com/micrium-examples/category/240-stm3240g-eval

#### Step 3.2: Including Micrium's Handlers
- Micrium's own PendSV and SysTick interrupt handlers need to be used.
    + The startup assembly file startup_stm32f4...s is modified and Micrium's handlers are added to the vector table: 
    + All occurances of PendSV_Handler are replaced with OS_CPU_PendSVHandler.
    + All occurances of SysTick_Handler are replaced with OS_CPU_SysTickHandler.

#### Step 3.3: Testing the Integration
- An example LED blink program is implemented according to the multi-task application example from Getting Started section of the book.
- It is very important to initialize SysTick with "OS_CPU_SysTickInit" function call. 
    + This function has to be called after the OSStart call, in other words, inside the starter task function.
    + The argument to the function is number of clock ticks between each OS tick interrupt. For example, in order to achieve a period of 1ms, clock frequency value is divided by 1000 and given as argument.
    + An alternative to this function is OS_CPU_SysTickInitFreq, where the CPU frequency is given as argument and the intended tick rate is specified via OS_CFG_TICK_RATE_HZ configuration constant in os_cfg_app.h file.
- The program is verified both by debugger and observing blinking LEDs.
<br/>


### Step 4: Adding SystemView Source Code
Related commit: [SystemView Source Code Added](https://github.com/hasanmutlu26/STM32-Micrium-ucOSIII-with-SystemView/commit/36f94846f33e453d8fdccb6f31afebaaba28a55a)

- SystemView source code is added to the according to the recommended structure in SystemView User Guide, "Getting started with SystemView on the target" chapter.
- Additionally, SEGGER/SEGGER_RTT_ASM_ARMv7M.S file is also included since the processor is ARMv7M based.

<br/>

### Step 5: Completing SystemView Integration
Related commit: [Configuration and First SystemView Integration](https://github.com/hasanmutlu26/STM32-Micrium-ucOSIII-with-SystemView/commit/18e9b4dbd0737d07ff4f9c1ca11002aa735c65e8)

- Configurations are made in Micrium's os_cfg.h configuration header file.
    + OS_CFG_DBG_EN and OS_CFG_TRACE_xxxx_EN constants are enabled. 
- Configurations are made in SystemView's SEGGER_SYSVIEW_Config_uCOSIII.c configuration header file.
    + SYSVIEW_TIMESTAMP_FREQ constant is defined as CPU_TS_TmrFreqGet(&local_err). The local_err is created as a CPU_ERR variable.
    + SYSVIEW_CPU_FREQ constant is defined as SystemCoreClock. stm32f4xx.h header file is included to use it. 
- The SEGGER_SYSVIEW.h is included in the main.c.
- SEGGER_SYSVIEW_Conf() and SEGGER_SYSVIEW_Start() functions are called just after the OSInit() call in main.c

<br/>

### Step 6: Testing the Integration
Related commit: [Configuration and First SystemView Integration](https://github.com/hasanmutlu26/STM32-Micrium-ucOSIII-with-SystemView/commit/18e9b4dbd0737d07ff4f9c1ca11002aa735c65e8)

- In order to use SystemView, the on-board ST-Link needs to be converted to J-Link using SEGGER's STLinkReflash utility. This program is also able to restore it back to ST-Link.
    + Before using the tool, the following page of ST-Link Reflash need to be read carefully: https://www.segger.com/products/debug-probes/j-link/models/other-j-links/st-link-on-board/
    + When prerequisites are met, ST-Link USB drivers and J-Link software package are installed, the program can be used.
    + Despite not being listed in Compatible Evaluation Boards, it is tested and worked without any issue on the Nucleo-F439ZI board.
- ST-Link is converted to J-Link using the STLinkReflash.
- SystemView is tested in real-time capture mode.
    + The application is started in debugger mode.
    + A breakpoint is put in the first statement after SEGGER_SYSVIEW_Start() function call and the program is resumed.
    + When program stops in the breakpoint, SystemView real-time tracing is started and then the program is resumed.

<br/>

### Step 7: Capturing ISRs
Related commit: [Interrupt Record Functionality Added](https://github.com/hasanmutlu26/STM32-Micrium-ucOSIII-with-SystemView/commit/d97a88736b18af54acb42aebe8de21e178a121a9#diff-5e5e5b336a54fbbccee93189b80630ace669bf72165dce8f6d6e3cd740faedd0)

+ In order to record interrupts, SEGGER_SYSVIEW_RecordEnterISR() and SEGGER_SYSVIEW_RecordExitISR() calls need to be added to the beginning and ending of the interrupt handler function.
    - It is tested by creating a simple user button interrupt. 