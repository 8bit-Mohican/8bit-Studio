.import ip65_init

.export init_network,_init_network

init_network:
.proc _init_network
  jsr ip65_init
  rts
.endproc

  