.import ip65_init
;.import ip65_process

;.import cfg_mac
;.import cfg_ip
;.import cfg_netmask
;.import cfg_gateway
;.import cfg_dns
;.import cfg_tftp_server

.import dhcp_init
;.import dhcp_server

.ifdef A2_SLOT_SCAN
.import a2_set_slot
.endif

.macro init_ip_via_dhcp
.ifdef A2_SLOT_SCAN
  lda #1
: pha
  jsr a2_set_slot
  jsr ip65_init
  pla
  bcc :+
  adc #0
  cmp #8
  bcc :-
:
.else
  jsr ip65_init
.endif
  php
  ;print_driver_init
  plp
  bcc :+
  ;print_failed
  sec
  jmp @end_macro
: ;print_ok
  ;print_dhcp_init
  jsr dhcp_init
  bcc :+
  ;print_failed
  sec
  jmp @end_macro
: ;print_ok
  clc
@end_macro:
.endmacro

.export init_network,_init_network

init_network:
.proc _init_network
  init_ip_via_dhcp
  rts
.endproc