These drivers are not complete and littered with bugs. Just functional enough to recover my ECU's after I've f-ed for the millionth time while working on bootloaders. Hence the speed, which is my only concern. I _really_ hate the waiting part..


-:Flash:-

T5:
39sf020: 4,64 secs(Only 256K), tn28f010: 8,21, cat28f010: 8,18, am28f010: 8,62, at29c010: 4~ Secs.

Note on Atmel: I've only flashed these with Thingy 2.0 (unreleased) Expect it to be slower with the included test host

T7: 11,37, T8: 22,32, MCP: 14,1 - 37,2 Secs.

-:Dump:-

T5: 2.35 Secs, T7: 4,61, t8: 6,92

I know with 100% certainty these times can be improved if you copy and paste some code from T5Legion. I'm just lazy :)

If you build one of these yourself, make sure to use GOOD cables and they must also be short. Regular jumpercables are not good enough.
I'm pretty much pushing every limit there is so the slightest interference is enough to throw the host off. This is especially true on Trionic 8 and MCP since it goes full blast, 8 MHz, on the SPI interface. (Yep, SPI is used in a weird way to implement BDM)
