- run minicom setup:
  ```console
  sudo minicom -s
  ```
- navigate to `Serial port setup`
- select `Serial Device` by pressing the `A` key
  - insert the correct USB peripheral, in our case it is `/dev/ttyUSB1`
  - if you are unsure, check which peripheral appears under the `/dev` directory when you plug in the PYNQ board
- press `enter` to exit the selection
  - depending on the verision of minicom you might have two or more options that have a yes/no setting
  - make sure you set every one to `no`
- press `enter` again to exit the `Serial port setup`
- navigate to `Save setup as dfl` and press `enter`
- navigate to `Exit from Minicom` and press `enter`
