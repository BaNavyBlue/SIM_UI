# SIM_UI
The user interface and control code for the SARA LAB MF-SIM system.

The UI is build using the cycfi Elements library.
https://github.com/cycfi/elements

And must be built using their repo.
Many of the main interface functions I will attempt to write as generically as possible so they can be modified to USE any devices API/SDK.

Currently this code is being developed to use the Meadowlark Blink SDK for controlling our SLM. I will not include any of their libraries since they aren't open source.

It will also have code to manipulate a PI stage, and thorlabs. We will not include any of their libraries either unless they are opensource.
