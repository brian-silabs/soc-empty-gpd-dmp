# soc-empty-gpd-dmp (BLE & GPD DMP)

## Description
This projects aims to be a simple demonstrator of Silabs GPD implementation over Micrium OS, aside Silabs BLE Stack
I tried to use the majority of the existing code to avoid any trouble in importing the future GPD stacks 

## What has been done
  1. Used soc-empty-rail-dmp as reference
  2. Replaced the BLE project's libmbedtls by the library itself
    * Followed the guide here: [put KBA link]
    * Added MbedTLS Threading module to share one crypto block between stacks (for portability)
  3. Added RAC_TX and RAC_RX signals output to PC10 and PC11 (Exp Header #15 & #16)
  4. Moved the main loop of the GPD to a Micrium task
  5. Changed the Sequencer implementation, making it clearer
  6. Added A GPD Control Service to the BLE GATT database with 3 characteristics:
    * Commission command
    * Toggle command
    * Decommission command

## Operation
The device starts advertising in discoverable and connectable mode
When operating the GP Commissioning, the BLE activity is stopped
Advertising/Connectivity gets back after that (TODO)

Ideally, radio scheduler priorities need to be handled so that both protocols can coexist

## Work to be done
1. Repair NVM - use the BLE one
2. Code feedback from GPD to BLE App
3. Radio scheduler priorities need to be handled so that both protocols can coexist
4. Check Low power operation (BLE stack might not see the GPD activity and shut down the device - not tested)

## Notes
NB: GPD Channel for RX during 1st commissioning phase is always 11 (channel configuration channel)
NB: Main loop has to be periodically called so the device behavior works (Approx 1 sec between each)

## Fixed issues :
1. An issue where Commissioning Requests are sent right after each other
   The Sink does not have time to send back the Comm Reply in time

## Known issues :
  * The GP Sink Sends the Commissioning Repliess too late (~ 74ms)
  * The GPD takes the 1st frames it receives upon RX window.
    If it is a wrong frame, another RXafterTX cycle is needed

