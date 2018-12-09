# nexstar-com

Drivers and interface for NexStar-compatible motor controllers, typically found on Celestron telescope mounts. Note that some NexStar models have a separate motor controller and hand controller interface, and these follow separate protocols, though both are relatively easy to program.

This controller operates on a virtual COM port over USB, so you will need to buy or build a USB to RS-232 adapter. It is relatively straightforward to modify the code to transmit over a native COM port, as well.
