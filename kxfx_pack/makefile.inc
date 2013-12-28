# kX Driver
# Copyright (c) Eugene Gavrilov, 2001-2012
# All rights reserved

!include "../makefile.inc"

install_timebalance_kxs: $(DONE)\kxTimeBalanceV2A.kxs $(DONE)\kxTimeBalanceV2B.kxs
	$(SAY) Installed kxTimeBalanceV2B.kxs and kxTimeBalanceV2A.kxs

$(DONE)\kxTimeBalanceV2B.kxs: kxTimeBalanceV2B.kxs
	$(COPY) kxTimeBalanceV2B.kxs $(DONE)\kxTimeBalanceV2B.kxs

$(DONE)\kxTimeBalanceV2A.kxs: kxTimeBalanceV2A.kxs
	$(COPY) kxTimeBalanceV2A.kxs $(DONE)\kxTimeBalanceV2A.kxs
