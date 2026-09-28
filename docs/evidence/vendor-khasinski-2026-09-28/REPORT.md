# Vendor import — khasinski/parasite-eve-decomp (2026-09-28)

Upstream https://github.com/khasinski/parasite-eve-decomp @ `3067919e7b82f0334b873117eef07e7157f4ed69`, attribution khasinski (Chris Hasinski).
Imported under the owner's explicit authorization (see vendor/khasinski-parasite-eve-decomp/PROVENANCE.md).
Every file carries the `vendor-derived:` header. Method: vendor TU -> era cpp with vendor CPP flags (-undef -P),
vendor names mapped to address names; `era_link_check.py --target <id>` LINK_EXACT, then
`disc1_preflight.py --target <id> --deep --only <funcs>` PASS and `scripts/exact_rebuild_overlay.sh <id>` PASS.

* Overlay rows: **422** in 56 targets; distinct bodies **326** (60 plain C, 266 using the vendor's inline GTE asm macros — flagged `inline_gte`).
* EXE leaves: **1** (func_80014BA0).

## Gate lines (last per target)

* `EXACT_REBUILD_GATE[room_m0005i]=PASS plan=e9a600e9d782f32f70330f68258306363d2dc069fc1545a1d35ccdec13961a4a yaml=52913c5be55837663e648942341b4fe4820f4faf9015aed29555ec6d5bf5c9cb spans=[2 c, 1 asm, 0 rodata, 2 bin] sha1_orig=13b5953b0d09977a54c0ea43ab5c944cd208da20 sha1_cand=13b5953b0d09977a54c0ea43ab5c944cd208da20 sha1=13b5953b0d09977a54c0ea43ab5c944cd208da20`
* `EXACT_REBUILD_GATE[room_m0014i]=PASS plan=68a6a9e0014934e879fe392b5d0bd0636caccdd919037c18a179da13627d4cb8 yaml=48a69178739eea1f461182ac64070e5cd96510343fee3c87a4126fbc2a731d02 spans=[54 c, 2 asm, 1 rodata, 2 bin] sha1_orig=af4054c825b84d32b0c9a49131a50aa02bf974b9 sha1_cand=af4054c825b84d32b0c9a49131a50aa02bf974b9 sha1=af4054c825b84d32b0c9a49131a50aa02bf974b9`
* `EXACT_REBUILD_GATE[room_m0017i]=PASS plan=29fcc4f3d8b5f30f0ea1b7dfe4fa2ba3546921e35d8ff5f4da52947334c63f2f yaml=fc9e06906f80f7d1739b228362738cf253a6516b87355783701e3b1452987735 spans=[5 c, 1 asm, 0 rodata, 2 bin] sha1_orig=b40116e92e76cdcbc81a583c79ab2513c369fe64 sha1_cand=b40116e92e76cdcbc81a583c79ab2513c369fe64 sha1=b40116e92e76cdcbc81a583c79ab2513c369fe64`
* `EXACT_REBUILD_GATE[room_m0022i]=PASS plan=fce29040206832ab780f99e538a063be52dc1f63bc7923db815cab2427721321 yaml=e2aa7a835ece00bb44ae1bb09626e8b3b5d5be6accfd4ef40c69371bc1fcac7b spans=[53 c, 0 asm, 1 rodata, 2 bin] sha1_orig=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1_cand=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3 sha1=3231b287bdde6fb55bcc3734f5be6ff4906ab0c3`
* `EXACT_REBUILD_GATE[room_m0049i]=PASS plan=f25f7a5864ea9d5e458ca162519569c2c04c6dc973b1fbb3052fc42036724294 yaml=1d0750854e4bdd1a8854e0f988c03268236b316b3d8da7abd734bd46c12242b1 spans=[27 c, 0 asm, 0 rodata, 2 bin] sha1_orig=320861e23df3605a60e159b7db5fc081aa421607 sha1_cand=320861e23df3605a60e159b7db5fc081aa421607 sha1=320861e23df3605a60e159b7db5fc081aa421607`
* `EXACT_REBUILD_GATE[room_m0065i]=PASS plan=7edc92dc8ac40fb3cec5f0a8dc066b75148e98aab3022f567259fef8bb792818 yaml=3be3c90be200cc2139707fff340badfb99b2368fa9a0fb0ccc63eccc4bd4640c spans=[13 c, 0 asm, 0 rodata, 2 bin] sha1_orig=fa62e6cdf74b19728ab31b55f190143f0ec93bc8 sha1_cand=fa62e6cdf74b19728ab31b55f190143f0ec93bc8 sha1=fa62e6cdf74b19728ab31b55f190143f0ec93bc8`
* `EXACT_REBUILD_GATE[room_m0075i]=PASS plan=a53896b0926d2ba90619f7f749fe8fbc00d8fc58f0cd9c08ec15482f6f5c06d1 yaml=6faa55fc2c01c50ceb0192570aa83bc1ab15500f83ceb1e7bde765d2128af1e2 spans=[73 c, 2 asm, 1 rodata, 2 bin] sha1_orig=ca34d1321e158763e9ee8b3cc9df0b3d94d28978 sha1_cand=ca34d1321e158763e9ee8b3cc9df0b3d94d28978 sha1=ca34d1321e158763e9ee8b3cc9df0b3d94d28978`
* `EXACT_REBUILD_GATE[room_m0086i]=PASS plan=651bec6d97d619686dde711d48d0bddeb2a0819ad8e29d332e1d4b2c8d58526d yaml=74e9ad7d918830fc08d994f877a046c43e181f3943ba41ce37da00cd824e7283 spans=[3 c, 1 asm, 0 rodata, 2 bin] sha1_orig=1ebeb226defaf851ce2735245f028d4b63b1ec24 sha1_cand=1ebeb226defaf851ce2735245f028d4b63b1ec24 sha1=1ebeb226defaf851ce2735245f028d4b63b1ec24`
* `EXACT_REBUILD_GATE[room_m0087i]=PASS plan=0d3121bdbeefe75796898294ca57969322a02be08a4406b113b898d50759c88d yaml=dc630ba8116499dffe817cb7ff89c72f35bd47d17ed24f8513bce492d9d455f3 spans=[66 c, 0 asm, 1 rodata, 2 bin] sha1_orig=4c6107aa1de07a456f12d3723abab64156f4e73b sha1_cand=4c6107aa1de07a456f12d3723abab64156f4e73b sha1=4c6107aa1de07a456f12d3723abab64156f4e73b`
* `EXACT_REBUILD_GATE[room_m0089i]=PASS plan=b8f17c2360626c736d6392b800b8940b17a0f51a7243f30c2b49a8d6675b0d12 yaml=2b1d5b33209f7c7bfb5975627899449f402cdf85ad0e3ab245b5fc9a07757b7e spans=[54 c, 3 asm, 1 rodata, 2 bin] sha1_orig=06bee6ec7422f225a5ea845d3ff33937736253ca sha1_cand=06bee6ec7422f225a5ea845d3ff33937736253ca sha1=06bee6ec7422f225a5ea845d3ff33937736253ca`
* `EXACT_REBUILD_GATE[room_m0104i]=PASS plan=dab17184968d504d716a841fc8de9171ba8bc8816e78e43e8a93948e5fefcdbd yaml=720741323938e70923503da7c14f7ced14f1640019f2a84aa849526140c27e64 spans=[2 c, 1 asm, 0 rodata, 2 bin] sha1_orig=6ce13ec8a95c5114e70950d66d24e82a00b201e9 sha1_cand=6ce13ec8a95c5114e70950d66d24e82a00b201e9 sha1=6ce13ec8a95c5114e70950d66d24e82a00b201e9`
* `EXACT_REBUILD_GATE[room_m0107i]=PASS plan=44ef0f8425246d2e6dc74ee2ff9efc8ee6d8388be84b4e245e65704f21914565 yaml=46f37d2da956d78a02914eb1c0b98ea06fe6bd70e5b2d8cda5b7d3923c2b51c7 spans=[75 c, 0 asm, 1 rodata, 2 bin] sha1_orig=dcf6cf737f829adb79fad3ecb65a08d57cf6f664 sha1_cand=dcf6cf737f829adb79fad3ecb65a08d57cf6f664 sha1=dcf6cf737f829adb79fad3ecb65a08d57cf6f664`
* `EXACT_REBUILD_GATE[room_m0111i]=PASS plan=3ce46d74110b6662a2389a3120c1db7da010f70e97c6f5b40dea7684c59da408 yaml=d7d3f73282187cf2d1f3b5aa46bf21a378e782673d8672726c83474e7df8c042 spans=[22 c, 0 asm, 0 rodata, 2 bin] sha1_orig=a360b6aa799167c27616a596cf75fe006d635809 sha1_cand=a360b6aa799167c27616a596cf75fe006d635809 sha1=a360b6aa799167c27616a596cf75fe006d635809`
* `EXACT_REBUILD_GATE[room_m0113i]=PASS plan=7300e2a6ab601fe2ce313f104f5c9a4e7c04b172fee4f64eb86a701dc3cd1cb2 yaml=bbf2dddea7caab9cae40456fbe1d7a5eaa31e94de8978cae3fc6a4b1f60ec747 spans=[4 c, 2 asm, 0 rodata, 2 bin] sha1_orig=f55e2fa3c9c13b158e03cbd30c9677b69e141422 sha1_cand=f55e2fa3c9c13b158e03cbd30c9677b69e141422 sha1=f55e2fa3c9c13b158e03cbd30c9677b69e141422`
* `EXACT_REBUILD_GATE[room_m0116i]=PASS plan=189eac4c7c30c6b66305b040b640f00cd161f2716a0db56bd780c83a620be778 yaml=4755a89cbe3c8251b8da4064860aead986f5100826a84fc5e52301c8feaacce2 spans=[54 c, 2 asm, 1 rodata, 2 bin] sha1_orig=cfc9e33ffd9b52879626a01bac21366e462212f1 sha1_cand=cfc9e33ffd9b52879626a01bac21366e462212f1 sha1=cfc9e33ffd9b52879626a01bac21366e462212f1`
* `EXACT_REBUILD_GATE[room_m0122i]=PASS plan=6f3b92e6901720643723b2243d2e22d0c33352f1c073ba81e759e9203ff65726 yaml=f4fbb1d0b8bc5704b5c0c1e4cef90d5d177311b33e29d1ffca63ef5866cd666f spans=[24 c, 1 asm, 0 rodata, 2 bin] sha1_orig=1aae0fbca4b21638827d3b304516802937e1488f sha1_cand=1aae0fbca4b21638827d3b304516802937e1488f sha1=1aae0fbca4b21638827d3b304516802937e1488f`
* `EXACT_REBUILD_GATE[room_m0123i]=PASS plan=720ee6fa0809367068b07ff247e50c0258f9580d21c5838255240c51a220f561 yaml=b29a58e2df8a69d5d30662016c8eac55886af481e7f656f84d4c2a98d2adafc1 spans=[62 c, 3 asm, 1 rodata, 2 bin] sha1_orig=7dbc62ff99461836f22d884ef04718af387e04cd sha1_cand=7dbc62ff99461836f22d884ef04718af387e04cd sha1=7dbc62ff99461836f22d884ef04718af387e04cd`
* `EXACT_REBUILD_GATE[room_m0141i]=PASS plan=765858302f291f7e221aee1f985cbfda8173bdba9950451afe2840e034c67dac yaml=b6ee47f60a7088c8d4403b9636e91eaf08bc49e7734c9b015af00f721cd28fe5 spans=[30 c, 2 asm, 0 rodata, 2 bin] sha1_orig=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc sha1_cand=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc sha1=63c6c6ceb406be45d2a1d23a6bc0bb8dcd45b0fc`
* `EXACT_REBUILD_GATE[room_m0145i]=PASS plan=b5cdee95b98a5bc5f19e5572a148c9284e2d10f48a5c2cc658d07a4cc1b9c956 yaml=796f13c424324146f3b2d721c46fe94f89e1281df783ad7e86cedf37d8ef19ff spans=[63 c, 1 asm, 1 rodata, 3 bin] sha1_orig=8bb899c982ce00e9d43ca6807f0f8aa770f36de7 sha1_cand=8bb899c982ce00e9d43ca6807f0f8aa770f36de7 sha1=8bb899c982ce00e9d43ca6807f0f8aa770f36de7`
* `EXACT_REBUILD_GATE[room_m0146i]=PASS plan=5bd322b011fbc49dd9005f8b14e7c306b266ec5c2bb69946205782f38155f6bd yaml=ec6d73963e7ad6a9ad4df3f1646d17be8718e3c7babd059fc63a6b38442c440e spans=[49 c, 1 asm, 0 rodata, 2 bin] sha1_orig=72a6cf583e2916e84a5d452ae90a444d19688f68 sha1_cand=72a6cf583e2916e84a5d452ae90a444d19688f68 sha1=72a6cf583e2916e84a5d452ae90a444d19688f68`
* `EXACT_REBUILD_GATE[room_m0152i]=PASS plan=aad2f35d842f6344833b7ca259996816fa29121c7901dfc807392e0b6d234021 yaml=01982f85c212da6e31b15ee88e893df222fbc4b85ef534c8e4a616399cecf81e spans=[65 c, 2 asm, 1 rodata, 2 bin] sha1_orig=3f71f680b594ffbcb7a2f1364a8ef33152bda33d sha1_cand=3f71f680b594ffbcb7a2f1364a8ef33152bda33d sha1=3f71f680b594ffbcb7a2f1364a8ef33152bda33d`
* `EXACT_REBUILD_GATE[room_m0153i]=PASS plan=4a73cc22c266634c910342b84690e094eec6663282a1c25c76e82fe520248de8 yaml=14ece2af500d26012f2cb3e4922823015bd826d194ec66ab6d6736dd05c2f113 spans=[91 c, 2 asm, 1 rodata, 3 bin] sha1_orig=342c8ca376fc14e129164ce826e2a6d867602eb2 sha1_cand=342c8ca376fc14e129164ce826e2a6d867602eb2 sha1=342c8ca376fc14e129164ce826e2a6d867602eb2`
* `EXACT_REBUILD_GATE[room_m0156i]=PASS plan=bc13203cb8e097808ff7465cc4d2113e1e71612f7288c20a39f364ac98b4e928 yaml=c0eb3c3abbc37b17ec68495b497f4cd700dc6b361633ddc115bb2cb6a842b493 spans=[6 c, 3 asm, 0 rodata, 2 bin] sha1_orig=2d094b8b74b918379e1ed7c213e1337fb7fe4cac sha1_cand=2d094b8b74b918379e1ed7c213e1337fb7fe4cac sha1=2d094b8b74b918379e1ed7c213e1337fb7fe4cac`
* `EXACT_REBUILD_GATE[room_m0162i]=PASS plan=6c30ead2c7c5014a4c2bfa5d338f08e0d4e2e5b79ca497024fbda975fbcfe911 yaml=80a55a60875003822a3f05808d578e998804907f5cd9abb5d0e90449f5e8d82c spans=[53 c, 2 asm, 1 rodata, 2 bin] sha1_orig=35983d5c944d5fd25adcfb06e7f63b6251ab845d sha1_cand=35983d5c944d5fd25adcfb06e7f63b6251ab845d sha1=35983d5c944d5fd25adcfb06e7f63b6251ab845d`
* `EXACT_REBUILD_GATE[room_m0167i]=PASS plan=f562f1e3a5fe62089dd8a1b373c8e59628f7a960b523da8cf9942d61fcdf62cd yaml=2f43bc609a2f83bc44a54403ab2ddbf6e875c1146f68895d9ed2ec10a23d3fb3 spans=[66 c, 0 asm, 1 rodata, 2 bin] sha1_orig=8414175491c6dc4ee53bde60f2801265aa64bcfc sha1_cand=8414175491c6dc4ee53bde60f2801265aa64bcfc sha1=8414175491c6dc4ee53bde60f2801265aa64bcfc`
* `EXACT_REBUILD_GATE[room_m0174i]=PASS plan=eeb0e51852a0e0151ada2c1133d0ffcdc49d71d4a162ee80fbb29065967b424b yaml=e47b142ec9e9814c7b8a441f13db38267f9045888758d1aa0aa7704891559ccc spans=[113 c, 4 asm, 1 rodata, 2 bin] sha1_orig=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01 sha1_cand=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01 sha1=b2f0329a9b0c1808f85437d846b4cc36bd9e6c01`
* `EXACT_REBUILD_GATE[room_m0186i]=PASS plan=46966e423cc457beb5de045b8ba75aa255865f16ed1301101396f83cb3d84f87 yaml=62808100f062a25286426e4838e88429f168a44e0e75e919c033c41d845b1fb1 spans=[84 c, 1 asm, 1 rodata, 2 bin] sha1_orig=55c712d21f3366076f3288a3003a1877b414056e sha1_cand=55c712d21f3366076f3288a3003a1877b414056e sha1=55c712d21f3366076f3288a3003a1877b414056e`
* `EXACT_REBUILD_GATE[room_m0187i]=PASS plan=8d218f7b253c8cb6b9a79d2a0a41a86c8f7319885484a248a05bdac0bb3ddd8c yaml=c1ceb7356675a3e79a3ccd7db58533a21211452740a27f1cec6053d637cfbb4b spans=[18 c, 0 asm, 0 rodata, 2 bin] sha1_orig=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1 sha1_cand=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1 sha1=24ba474b85899a088e5b87a5c8d30ca65ebc5ab1`
* `EXACT_REBUILD_GATE[room_m0188i]=PASS plan=3d77cf1800cb3a3676f34e488aec38a279d94a57974b75f5ddb6848d08d81086 yaml=d68cad2dea96db0ae34f965226d0799f6841a3a1de8fba7cb86e5ece58d2ad20 spans=[57 c, 5 asm, 1 rodata, 2 bin] sha1_orig=80f620421f1394d2e7de02265f484c54b98a9284 sha1_cand=80f620421f1394d2e7de02265f484c54b98a9284 sha1=80f620421f1394d2e7de02265f484c54b98a9284`
* `EXACT_REBUILD_GATE[room_m0203i]=PASS plan=3c62d434d3fe25ecf634c44fc2ba936f9b4e3063ac0fad15ed5bbccbf09bdda5 yaml=8d3298415eb2fde56bda670a660afc14b0d1ba68cc6257cd2b36b8a3d6922cab spans=[56 c, 1 asm, 1 rodata, 2 bin] sha1_orig=6f9743ef89e5fb16a761b32a00d167402203c376 sha1_cand=6f9743ef89e5fb16a761b32a00d167402203c376 sha1=6f9743ef89e5fb16a761b32a00d167402203c376`
* `EXACT_REBUILD_GATE[room_m0205i]=PASS plan=dc5007c31b2692e5b1c68057f4bf000feac38a85dddc372a561a226eb7b98308 yaml=166ff94928207cfa32c6f75e326e6787fdf47d9fe9a4e838c013148b7a8e0222 spans=[54 c, 2 asm, 1 rodata, 2 bin] sha1_orig=0c281041b45ca7461ec98c2536143f14a63ae43e sha1_cand=0c281041b45ca7461ec98c2536143f14a63ae43e sha1=0c281041b45ca7461ec98c2536143f14a63ae43e`
* `EXACT_REBUILD_GATE[room_m0231i]=PASS plan=80e2ed82f2e74cfc44f056a609e87080994b7304fa0e6899cf1c072bc72e6135 yaml=99e360062ea34cef23cf1a3ba97164bc6f14df5dd26317a6739a6daec44ca6c1 spans=[58 c, 2 asm, 1 rodata, 2 bin] sha1_orig=94400edc11b5d5e300c179482d99935456f0f918 sha1_cand=94400edc11b5d5e300c179482d99935456f0f918 sha1=94400edc11b5d5e300c179482d99935456f0f918`
* `EXACT_REBUILD_GATE[room_m0245i]=PASS plan=140f5ef69eb5528897859b42bf934ef2e192e6cb33ca03639bc3f1d5ededcc34 yaml=25713f4fba15c18d5bdf2c531be9ff02a03802395722df9d14450fc9511e9ce5 spans=[56 c, 4 asm, 1 rodata, 2 bin] sha1_orig=512b5a7c48188edf45d84476a9d02265a51d1708 sha1_cand=512b5a7c48188edf45d84476a9d02265a51d1708 sha1=512b5a7c48188edf45d84476a9d02265a51d1708`
* `EXACT_REBUILD_GATE[room_m0256i]=PASS plan=945e39d57b6d7804c58d652e768193c55463a0a947b3b8a9a9d773fa6a6747bd yaml=e118b64312894225bd531f9c72093ead40f8e30e1c28d4cfad43f37043dead35 spans=[65 c, 3 asm, 1 rodata, 2 bin] sha1_orig=618fb891329d4ac903ec617a210c4b1e0af742d1 sha1_cand=618fb891329d4ac903ec617a210c4b1e0af742d1 sha1=618fb891329d4ac903ec617a210c4b1e0af742d1`
* `EXACT_REBUILD_GATE[room_m0263i]=PASS plan=1843f8c7fe48f0d5c6e23268883f4625bc45ff69f58f625b5de785a54194e6aa yaml=3c3c02d8333fa99e4791315905ce41a99c902994ff2f9d7fb80b51b2cd2e582c spans=[56 c, 3 asm, 1 rodata, 2 bin] sha1_orig=02f1b9cdb8ebc50504b5622d1b663246c8427dea sha1_cand=02f1b9cdb8ebc50504b5622d1b663246c8427dea sha1=02f1b9cdb8ebc50504b5622d1b663246c8427dea`
* `EXACT_REBUILD_GATE[room_m0269i]=PASS plan=5382e8a7e37e85b9b120e6a174191dc1446f77fe06b1c2a5d1287d17b7cf89dc yaml=446b6676300cc9bf914102bd8abc168021d73487cbb2ed10fc84c6665745f5f1 spans=[14 c, 2 asm, 0 rodata, 2 bin] sha1_orig=06c90576dba22ef80781421d5e9c0e802c0a339a sha1_cand=06c90576dba22ef80781421d5e9c0e802c0a339a sha1=06c90576dba22ef80781421d5e9c0e802c0a339a`
* `EXACT_REBUILD_GATE[room_m0273i]=PASS plan=63c9f369ddc9cba55ce53773f443f0cfb75d7db66a420ebbd041a1bb0ccb2b7e yaml=2790d045c403977f294b14692016cdc02780eb737021468a52ce5b56ecd5c43f spans=[120 c, 9 asm, 1 rodata, 2 bin] sha1_orig=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a sha1_cand=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a sha1=692fdf1477e29d8eb1e2e6c8a7e1cc8ac51dda3a`
* `EXACT_REBUILD_GATE[room_m0318i]=PASS plan=5f5c239102f0ab7929cb3e3f6a7f1e02fb9173a5ec745da8cad0937104cd8de9 yaml=c37f8c0a347a2e47b48e88013c3aa7e3f84bb3de5ccc22aa5daecbaecf438a5d spans=[67 c, 4 asm, 1 rodata, 2 bin] sha1_orig=3deebe65a4bb7eb5d1a9ececd8296a47ea7b9e4b sha1_cand=3deebe65a4bb7eb5d1a9ececd8296a47ea7b9e4b sha1=3deebe65a4bb7eb5d1a9ececd8296a47ea7b9e4b`
* `EXACT_REBUILD_GATE[room_m0350i]=PASS plan=d6ae8b74cc7a16816e27cdc3d607c2e2e829a57feb84b39bfc214ad752af0d3f yaml=211844da1a277e1a76ba053a6153ca4dcd49cafd7f1aa431b93f39360ec4f31c spans=[108 c, 7 asm, 1 rodata, 2 bin] sha1_orig=e9597b000d287033f981cee44f3db65deb8696e1 sha1_cand=e9597b000d287033f981cee44f3db65deb8696e1 sha1=e9597b000d287033f981cee44f3db65deb8696e1`
* `EXACT_REBUILD_GATE[room_m0358i]=PASS plan=4235da5520b897480e07dd751ad2e28ab6e3073aca63aac61c2800931147ee0f yaml=6e7bebe7240127bce40a1763838e6423426f4dbdc04d4a7dd347d5bf0426cdc6 spans=[62 c, 1 asm, 1 rodata, 2 bin] sha1_orig=3fbd46633b22dc770516a833a8f2a711e544d382 sha1_cand=3fbd46633b22dc770516a833a8f2a711e544d382 sha1=3fbd46633b22dc770516a833a8f2a711e544d382`
* `EXACT_REBUILD_GATE[room_m0391i]=PASS plan=02f0d94e41e187345851104fde8c6603a83c3c0acff0e3eaf88a6056a227b2bc yaml=cd6b50e26d0ada924e104d478f43a31f109ed2e4ad4aed7e2a64aff78480395a spans=[67 c, 2 asm, 1 rodata, 2 bin] sha1_orig=327d5766f293ada3d33a96d655a20273a9b95145 sha1_cand=327d5766f293ada3d33a96d655a20273a9b95145 sha1=327d5766f293ada3d33a96d655a20273a9b95145`
* `EXACT_REBUILD_GATE[room_m0392i]=PASS plan=4847dcd02ec86c0226dbf2580fe31364dfbe4d4e848851afe60717d56fad1f3a yaml=7d686172f0a8989a17d6ac88e36e4d6c03bc3f46875c421db3e8fa36647def54 spans=[76 c, 2 asm, 1 rodata, 2 bin] sha1_orig=b51734dcce487ce7fb0e79610bd81ace8f2d8462 sha1_cand=b51734dcce487ce7fb0e79610bd81ace8f2d8462 sha1=b51734dcce487ce7fb0e79610bd81ace8f2d8462`
* `EXACT_REBUILD_GATE[room_m0393i]=PASS plan=189715e4c918e8154aa02f90a2c30114259b06dc0e0074a7f06397aea491f11a yaml=45382f5fcc7a671678e2deb322dbc2f70ba2b45e743382e9c9ade66a5b8dff3d spans=[88 c, 0 asm, 1 rodata, 2 bin] sha1_orig=cef7a5f2ad37426f7894315ec54d97526f322ee8 sha1_cand=cef7a5f2ad37426f7894315ec54d97526f322ee8 sha1=cef7a5f2ad37426f7894315ec54d97526f322ee8`
* `EXACT_REBUILD_GATE[room_m0399i]=PASS plan=c127c5ec9b5eec8eb4ef9ff2ea8e0b06282e0fd4f26be625eb08187ab893b967 yaml=19f2df53fd9e6885cf5fe5f5fb236a0542eb8d2c3ac767f27e5afe54b4e2e715 spans=[76 c, 1 asm, 1 rodata, 2 bin] sha1_orig=d05fc006543447f413ca8cd7c1c1f2de0335c6b2 sha1_cand=d05fc006543447f413ca8cd7c1c1f2de0335c6b2 sha1=d05fc006543447f413ca8cd7c1c1f2de0335c6b2`
* `EXACT_REBUILD_GATE[room_m0401i]=PASS plan=ac492d83e083f47f8581b4682907d486e5e62a97a724c7d71fa98862798f613f yaml=fe61dd455ce59b23ba9aac1877f36934785dfe7997ed25af21b46d658bedcee5 spans=[98 c, 1 asm, 1 rodata, 3 bin] sha1_orig=19d2caf765e89732886d5034b92d78fd12a3e441 sha1_cand=19d2caf765e89732886d5034b92d78fd12a3e441 sha1=19d2caf765e89732886d5034b92d78fd12a3e441`
* `EXACT_REBUILD_GATE[room_m0404i]=PASS plan=86295104149da6b8898d4e0dff60b6f1974455f3298abe5a11a4012d2fc0ebbd yaml=190ea29ade82c9864412bb8e64dfe26d7173be232d72b34780be180a999a4071 spans=[54 c, 3 asm, 1 rodata, 2 bin] sha1_orig=c611bd599af3f8e0961c1dc5016e31d3d30e3971 sha1_cand=c611bd599af3f8e0961c1dc5016e31d3d30e3971 sha1=c611bd599af3f8e0961c1dc5016e31d3d30e3971`
* `EXACT_REBUILD_GATE[room_m0412i]=PASS plan=ac3d2188fd344ff92de6e00652bb75e4c02a9dee57bac02d21a570b56e5b0610 yaml=1b96cf4c0f265a7a0965dd229147c1f2a980d826767b974010fad1664e0a168d spans=[81 c, 4 asm, 1 rodata, 2 bin] sha1_orig=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1_cand=673097c6bd36986925a38e6a12d2b039092ed9c8 sha1=673097c6bd36986925a38e6a12d2b039092ed9c8`
* `EXACT_REBUILD_GATE[room_m0413i]=PASS plan=29c0b35cddba6e1444468f244128a846d4fb0320d141d2f6b03e607b282a4944 yaml=687e4bf0900e74e29479b45b1ea9e2bb67e123103164eefd4116a4b69c2e4f87 spans=[53 c, 2 asm, 1 rodata, 2 bin] sha1_orig=6083b41a6287b0e1015436aad411298206a40a8e sha1_cand=6083b41a6287b0e1015436aad411298206a40a8e sha1=6083b41a6287b0e1015436aad411298206a40a8e`
* `EXACT_REBUILD_GATE[room_m0414i]=PASS plan=cce21c7963a00f239e0aa95fa52e36b5b697471b00cd6db4d779080fa713e708 yaml=484823ffddca3773cd7e69e9f6c62a85e49d68b747c374f70aa6b1340721bedd spans=[29 c, 2 asm, 0 rodata, 2 bin] sha1_orig=c1b8f25dd4ad748e16405bccf6692dcb2bef6467 sha1_cand=c1b8f25dd4ad748e16405bccf6692dcb2bef6467 sha1=c1b8f25dd4ad748e16405bccf6692dcb2bef6467`
* `EXACT_REBUILD_GATE[room_m0418i]=PASS plan=c994812e54c9c1e38c6a34283b20a0c7c0ffa49b3252e7113e2e1b29f0db5e38 yaml=8b20a0336dddd5be69574bf0a406cc65b8d95abc628cd60d53f7c898712bebc2 spans=[143 c, 6 asm, 1 rodata, 2 bin] sha1_orig=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca sha1_cand=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca sha1=03c8f1c6d2d439c08811fa94cf78e56cca8ca9ca`
* `EXACT_REBUILD_GATE[room_m0419i]=PASS plan=53c2c8892399abdf0ecfaa952367e70a45abd6a02c9b7769a20e353caf9afd1c yaml=2333c60379d388b7e6c417bf11de30df11ba8ecca78130abc9341e72859c3510 spans=[75 c, 3 asm, 1 rodata, 2 bin] sha1_orig=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a sha1_cand=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a sha1=3583c4ecd060f5d4ef6b8c7f7a65c58b4e65492a`
* `EXACT_REBUILD_GATE[room_m0421i]=PASS plan=8f2c346afdd250151aee81bce591bb797432996ef3672a8c760894884c927d7f yaml=c603116cb3ca764f4e53fc0ff990d49b042808b81663f7d16f04ede16d870cef spans=[26 c, 2 asm, 0 rodata, 2 bin] sha1_orig=983c83c8d658d2228dca5c1d0c5fc234ea401108 sha1_cand=983c83c8d658d2228dca5c1d0c5fc234ea401108 sha1=983c83c8d658d2228dca5c1d0c5fc234ea401108`
* `EXACT_REBUILD_GATE[room_m0424i]=PASS plan=4209fafc2f9ab23bd3bded6dea26f26d6aa7cd85e60ca2bc64cfa9e092fd080a yaml=53ea9538548ca85a4f784c1a4d3153f93bcfd2fe4e4e1f7ed8be28f24fbf6f63 spans=[24 c, 1 asm, 0 rodata, 2 bin] sha1_orig=5488eb751ec4c35eb8aaecc7521b73282c701041 sha1_cand=5488eb751ec4c35eb8aaecc7521b73282c701041 sha1=5488eb751ec4c35eb8aaecc7521b73282c701041`
* `EXACT_REBUILD_GATE[room_m0428i]=PASS plan=0e9f460ded6062e210eee47b5be6358b89b096255ac4d7b66a613a1f1f8b5916 yaml=4e4bbb461affb1b6469f80eb32ac8ae3d3605f0192e0e561372cde857ff2d6dd spans=[58 c, 4 asm, 1 rodata, 2 bin] sha1_orig=7a8a6b604e210fa28b8c3d018aa1773df65f0f4c sha1_cand=7a8a6b604e210fa28b8c3d018aa1773df65f0f4c sha1=7a8a6b604e210fa28b8c3d018aa1773df65f0f4c`
* `EXACT_REBUILD_GATE[room_m0429i]=PASS plan=3e88c05ad258136369fb650360c8b6a5871e6b84e3c4eb6cf564394f73432abb yaml=26ebd01dcd2326cbe93ddfaabe3debd820658058b610c3ff20d6a81bd07d92d6 spans=[72 c, 3 asm, 1 rodata, 2 bin] sha1_orig=1487c56a4d420ebe6a13bb261622428226a66034 sha1_cand=1487c56a4d420ebe6a13bb261622428226a66034 sha1=1487c56a4d420ebe6a13bb261622428226a66034`
* `EXACT_REBUILD_GATE[room_m0432i]=PASS plan=7bfe1758e473ff23d69418abb436174f3cc852f0e6f5951c47ee55a1337bfc5a yaml=e360d369289975e966eec7a3979311bafb119b66a20963bc9b08a1ccdefaeae7 spans=[58 c, 3 asm, 1 rodata, 2 bin] sha1_orig=e13a8b54116474ab22fa0ae909a264442944ae4f sha1_cand=e13a8b54116474ab22fa0ae909a264442944ae4f sha1=e13a8b54116474ab22fa0ae909a264442944ae4f`

## Rows

| target | func | size | inline GTE | profile flags/env | vendor path |
|---|---|---|---|---|---|
| room_m0005i | func_8018F018 | 0x318 |  | -O2 -G0  | src/overlays/room_m005/RoomM005_FxOrbiter_8018F018.c |
| room_m0014i | func_8018F640 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_HandlerD.c |
| room_m0014i | func_8018FC68 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0014i | func_80190380 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_HandlerE.c |
| room_m0014i | func_80190D7C | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_HandlerB.c |
| room_m0014i | func_80191110 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_AdvanceArcToTarget_80191110.c |
| room_m0014i | func_80191894 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_HandlerC.c |
| room_m0014i | func_80191D18 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_AdvanceArcToTargetY_80191D18.c |
| room_m0014i | func_80192354 | 0x180 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_MoveActorLocal_80192354.c |
| room_m0017i | func_8018F1C4 | 0x34 |  | -O2 -G0  | src/overlays/room_m017/RoomEnt_Setup5_8018F1C4.c |
| room_m0022i | func_8018F630 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/RoomLib_HandlerD.c |
| room_m0022i | func_8018FC58 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0022i | func_80190370 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/RoomLib_HandlerE.c |
| room_m0022i | func_80190D6C | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/RoomLib_HandlerB.c |
| room_m0022i | func_80191100 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/func_80191100.c |
| room_m0022i | func_80191884 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/RoomLib_HandlerC.c |
| room_m0022i | func_80191D08 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/func_80191D08.c |
| room_m0022i | func_80192344 | 0x180 | yes | -O2 -G0  | src/overlays/room_m022/RoomLib_MoveActorLocal_80192344.c |
| room_m0049i | func_8018F13C | 0x230 |  | -O2 -G0  | src/overlays/room_m049/func_8018F13C.c |
| room_m0049i | func_8018F374 | 0x1034 |  | -O2 -G0  | src/overlays/room_m049/func_8018F374.c |
| room_m0049i | func_80190428 | 0x440 |  | -O2 -G0  | src/overlays/room_m049/func_80190428.c |
| room_m0049i | func_80190868 | 0x390 |  | -O2 -G0  | src/overlays/room_m049/RoomLib_UpdatePairedSprite_80190868.c |
| room_m0049i | func_80190CB0 | 0x490 |  | -O2 -G0  | src/overlays/room_m049/func_80190CB0.c |
| room_m0049i | func_80191140 | 0x3b0 |  | -O2 -G0  | src/overlays/room_m049/RoomLib_UpdatePairedSpriteBurst_80191140.c |
| room_m0049i | func_801915A8 | 0x3b0 |  | -O2 -G0  | src/overlays/room_m049/func_801915A8.c |
| room_m0049i | func_80191958 | 0x33c |  | -O2 -G0  | src/overlays/room_m049/RoomLib_UpdateSparkleHeight_80191958.c |
| room_m0049i | func_80191D44 | 0x434 |  | -O2 -G0  | src/overlays/room_m049/func_80191D44.c |
| room_m0049i | func_80192178 | 0x31c |  | -O2 -G0  | src/overlays/room_m049/RoomLib_UpdateTransformedSparkle_80192178.c |
| room_m0049i | func_8019249C | 0x1e4 |  | -O2 -G0  | src/overlays/room_m049/func_8019249C.c |
| room_m0049i | func_80192794 | 0x21c |  | -O2 -G0  | src/overlays/room_m049/RoomLib_DrawScaledEffect_80192794.c |
| room_m0049i | func_801929B0 | 0x1ac |  | -O2 -G0  | src/overlays/room_m049/func_801929B0.c |
| room_m0065i | func_8018F388 | 0x4a0 | yes | -O2 -G0  | src/overlays/room_m065/func_8018F388.c |
| room_m0075i | func_80191068 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m075/RoomLib_HandlerD.c |
| room_m0075i | func_80191690 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0075i | func_80191DA8 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m075/RoomLib_HandlerE.c |
| room_m0075i | func_801927A4 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m075/RoomLib_HandlerB.c |
| room_m0075i | func_80192B38 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m075/func_80192B38.c |
| room_m0075i | func_801932BC | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m075/RoomLib_HandlerC.c |
| room_m0075i | func_80193740 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m075/func_80193740.c |
| room_m0075i | func_80193D7C | 0x180 | yes | -O2 -G0  | src/overlays/room_m075/RoomLib_MoveActorLocal_80193D7C.c |
| room_m0086i | func_80190574 | 0x4d0 |  | -O2 -G0  | src/overlays/room_m086/func_80190574.c |
| room_m0087i | func_8018F560 | 0x4a0 | yes | -O2 -G0  | src/overlays/room_m065/func_8018F388.c |
| room_m0087i | func_80190634 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m087/RoomLib_HandlerD.c |
| room_m0087i | func_80190C5C | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0087i | func_80191374 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m087/RoomLib_HandlerE.c |
| room_m0087i | func_80191D70 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m087/RoomLib_HandlerB.c |
| room_m0087i | func_80192104 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m087/func_80192104.c |
| room_m0087i | func_80192888 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m087/RoomLib_HandlerC.c |
| room_m0087i | func_80192D0C | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m087/func_80192D0C.c |
| room_m0087i | func_80193348 | 0x180 | yes | -O2 -G0  | src/overlays/room_m087/RoomLib_MoveActorLocal_80193348.c |
| room_m0089i | func_8018F640 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_HandlerD.c |
| room_m0089i | func_8018FC68 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0089i | func_80190380 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_HandlerE.c |
| room_m0089i | func_80190D7C | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_HandlerB.c |
| room_m0089i | func_80191110 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_AdvanceArcToTarget_80191110.c |
| room_m0089i | func_80191894 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_HandlerC.c |
| room_m0089i | func_80191D18 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m014/RoomLib_AdvanceArcToTargetY_80191D18.c |
| room_m0089i | func_80192354 | 0x180 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_MoveActorLocal_80192354.c |
| room_m0089i | func_80192A30 | 0x2a4 |  | -O2 -G0  | src/overlays/room_m089/RoomLib_SpawnRandomEffect_80192A30.c |
| room_m0104i | func_8018F008 | 0x39c | yes | -O2 -G0  | src/overlays/room_m104/func_8018F008.c |
| room_m0107i | func_8018F6D8 | 0x300 | yes | -O2 -G0  | src/overlays/room_m107/func_8018F6D8.c |
| room_m0107i | func_8018FEAC | 0x2cc | yes | -O2 -G0  | src/overlays/room_m107/func_8018FEAC.c |
| room_m0107i | func_801906D0 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m107/RoomLib_HandlerD.c |
| room_m0107i | func_80190CF8 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0107i | func_80191410 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m107/RoomLib_HandlerE.c |
| room_m0107i | func_80191E0C | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m107/RoomLib_HandlerB.c |
| room_m0107i | func_801921A0 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m107/func_801921A0.c |
| room_m0107i | func_80192924 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m107/RoomLib_HandlerC.c |
| room_m0107i | func_80192DA8 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m107/func_80192DA8.c |
| room_m0107i | func_801933E4 | 0x180 | yes | -O2 -G0  | src/overlays/room_m107/RoomLib_MoveActorLocal_801933E4.c |
| room_m0111i | func_8018F500 | 0x300 | yes | -O2 -G0  | src/overlays/room_m111/func_8018F500.c |
| room_m0111i | func_8018FCD4 | 0x2cc | yes | -O2 -G0  | src/overlays/room_m111/func_8018FCD4.c |
| room_m0113i | func_8018FC8C | 0x39c | yes | -O2 -G0  | src/overlays/room_m113/func_8018FC8C.c |
| room_m0116i | func_8018F644 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/RoomLib_HandlerD.c |
| room_m0116i | func_8018FC6C | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0116i | func_80190384 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/RoomLib_HandlerE.c |
| room_m0116i | func_80190D80 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/RoomLib_HandlerB.c |
| room_m0116i | func_80191114 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/func_80191114.c |
| room_m0116i | func_80191898 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/RoomLib_HandlerC.c |
| room_m0116i | func_80191D1C | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/func_80191D1C.c |
| room_m0116i | func_80192358 | 0x180 | yes | -O2 -G0  | src/overlays/room_m116/RoomLib_MoveActorLocal_80192358.c |
| room_m0116i | func_801924FC | 0x39c | yes | -O2 -G0  | src/overlays/room_m116/func_801924FC.c |
| room_m0122i | func_8018F514 | 0x300 | yes | -O2 -G0  | src/overlays/room_m122/func_8018F514.c |
| room_m0122i | func_8018FCE8 | 0x2cc | yes | -O2 -G0  | src/overlays/room_m122/func_8018FCE8.c |
| room_m0122i | func_801900A8 | 0x39c | yes | -O2 -G0  | src/overlays/room_m122/func_801900A8.c |
| room_m0123i | func_8018F664 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/RoomLib_HandlerD.c |
| room_m0123i | func_8018FC8C | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0123i | func_801903A4 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/RoomLib_HandlerE.c |
| room_m0123i | func_80190DA0 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/RoomLib_HandlerB.c |
| room_m0123i | func_80191134 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/func_80191134.c |
| room_m0123i | func_801918B8 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/RoomLib_HandlerC.c |
| room_m0123i | func_80191D3C | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/func_80191D3C.c |
| room_m0123i | func_80192378 | 0x180 | yes | -O2 -G0  | src/overlays/room_m123/RoomLib_MoveActorLocal_80192378.c |
| room_m0123i | func_80194768 | 0x308 |  | -O2 -G0  | src/overlays/room_m123/RoomEffect_M123ParticleController.c |
| room_m0123i | func_80194A70 | 0x194 | yes | -O2 -G0  | src/overlays/room_m123/RoomEffect_M123PulseCallback.c |
| room_m0123i | func_80194C04 | 0x364 |  | -O2 -G0  | src/overlays/room_m123/RoomEffect_M123PulseController.c |
| room_m0123i | func_80195114 | 0x23c |  | -O2 -G0  | src/overlays/room_m123/func_80195114.c |
| room_m0141i | func_8018F5FC | 0x278 | yes | -O2 -G0  | src/overlays/room_m141/func_8018F5FC.c |
| room_m0141i | func_8018FF1C | 0x190 | yes | -O2 -G0  | src/overlays/room_m141/RoomLib_DrawGroundSprite_8018FF1C.c |
| room_m0141i | func_80191C44 | 0x2f4 |  | -O2 -G0  | src/overlays/room_m141/func_80191C44.c |
| room_m0145i | func_8018F638 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m145/RoomLib_HandlerD.c |
| room_m0145i | func_8018FC60 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0145i | func_80190378 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m145/RoomLib_HandlerE.c |
| room_m0145i | func_80190D74 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m145/RoomLib_HandlerB.c |
| room_m0145i | func_80191108 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m145/func_80191108.c |
| room_m0145i | func_8019188C | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m145/RoomLib_HandlerC.c |
| room_m0145i | func_80191D10 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m145/func_80191D10.c |
| room_m0145i | func_8019234C | 0x180 | yes | -O2 -G0  | src/overlays/room_m145/RoomLib_MoveActorLocal_8019234C.c |
| room_m0145i | func_80192CE0 | 0x198 | yes | -O2 -G0  | src/overlays/room_m145/RoomLib_UpdateTimedRender_80192CE0.c |
| room_m0146i | func_8018F5F0 | 0x278 | yes | -O2 -G0  | src/overlays/room_m141/func_8018F5FC.c |
| room_m0146i | func_8018FF10 | 0x190 | yes | -O2 -G0  | src/overlays/room_m146/RoomLib_DrawGroundSprite_8018FF10.c |
| room_m0146i | func_801920B0 | 0x2f4 |  | -O2 -G0  | src/overlays/room_m146/func_801920B0.c |
| room_m0152i | func_8018F67C | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m152/RoomLib_HandlerD.c |
| room_m0152i | func_8018FCA4 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0152i | func_801903BC | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m152/RoomLib_HandlerE.c |
| room_m0152i | func_80190DB8 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m152/RoomLib_HandlerB.c |
| room_m0152i | func_8019114C | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m152/func_8019114C.c |
| room_m0152i | func_801918D0 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m152/RoomLib_HandlerC.c |
| room_m0152i | func_80191D54 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m152/func_80191D54.c |
| room_m0152i | func_80192390 | 0x180 | yes | -O2 -G0  | src/overlays/room_m152/RoomLib_MoveActorLocal_80192390.c |
| room_m0152i | func_801939C8 | 0x198 | yes | -O2 -G0  | src/overlays/room_m152/RoomLib_UpdateTimedRender_801939C8.c |
| room_m0153i | func_8018F798 | 0x278 | yes | -O2 -G0  | src/overlays/room_m141/func_8018F5FC.c |
| room_m0153i | func_801900B8 | 0x190 | yes | -O2 -G0  | src/overlays/room_m153/RoomLib_DrawGroundSprite_801900B8.c |
| room_m0153i | func_80190B4C | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m153/RoomLib_HandlerD.c |
| room_m0153i | func_80191174 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0153i | func_8019188C | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m153/RoomLib_HandlerE.c |
| room_m0153i | func_80192288 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m153/RoomLib_HandlerB.c |
| room_m0153i | func_8019261C | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m153/func_8019261C.c |
| room_m0153i | func_80192DA0 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m153/RoomLib_HandlerC.c |
| room_m0153i | func_80193224 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m153/func_80193224.c |
| room_m0153i | func_80193860 | 0x180 | yes | -O2 -G0  | src/overlays/room_m153/RoomLib_MoveActorLocal_80193860.c |
| room_m0153i | func_801941F4 | 0x198 | yes | -O2 -G0  | src/overlays/room_m153/RoomLib_UpdateTimedRender_801941F4.c |
| room_m0153i | func_80195124 | 0x2f4 |  | -O2 -G0  | src/overlays/room_m153/func_80195124.c |
| room_m0156i | func_80190230 | 0x2dc |  | -O2 -G0  | src/overlays/room_m156+2/RoomEffect_OrbitingPairController.c |
| room_m0156i | func_801908B0 | 0x348 |  | -O2 -G0  | src/overlays/room_m156+2/RoomEffect_RandomPairController.c |
| room_m0156i | func_80190BF8 | 0x39c | yes | -O2 -G0  | src/overlays/room_m156+2/RoomLib_UpdateBounceRender_80190BF8.c |
| room_m0162i | func_8018F644 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/RoomLib_HandlerD.c |
| room_m0162i | func_8018FC6C | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0162i | func_80190384 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/RoomLib_HandlerE.c |
| room_m0162i | func_80190D80 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/RoomLib_HandlerB.c |
| room_m0162i | func_80191114 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/func_80191114.c |
| room_m0162i | func_80191898 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/RoomLib_HandlerC.c |
| room_m0162i | func_80191D1C | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m116/func_80191D1C.c |
| room_m0162i | func_80192358 | 0x180 | yes | -O2 -G0  | src/overlays/room_m116/RoomLib_MoveActorLocal_80192358.c |
| room_m0167i | func_8018F578 | 0x340 | yes | -O2 -G0  | src/overlays/room_m167/func_8018F578.c |
| room_m0167i | func_8018F8B8 | 0x4b8 | yes | -O2 -G0  | src/overlays/room_m167/func_8018F8B8.c |
| room_m0167i | func_801902CC | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m167/RoomLib_HandlerD.c |
| room_m0167i | func_801908F4 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0167i | func_8019100C | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m167/RoomLib_HandlerE.c |
| room_m0167i | func_80191A08 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m167/RoomLib_HandlerB.c |
| room_m0167i | func_80191D9C | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m167/func_80191D9C.c |
| room_m0167i | func_80192520 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m167/RoomLib_HandlerC.c |
| room_m0167i | func_801929A4 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m167/func_801929A4.c |
| room_m0167i | func_80192FE0 | 0x180 | yes | -O2 -G0  | src/overlays/room_m167/RoomLib_MoveActorLocal_80192FE0.c |
| room_m0174i | func_80193DC4 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m174/RoomLib_HandlerD.c |
| room_m0174i | func_801943EC | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0174i | func_80194B04 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m174/RoomLib_HandlerE.c |
| room_m0174i | func_80195500 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m174/RoomLib_HandlerB.c |
| room_m0174i | func_80195894 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m174/func_80195894.c |
| room_m0174i | func_80196018 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m174/RoomLib_HandlerC.c |
| room_m0174i | func_8019649C | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m174/func_8019649C.c |
| room_m0174i | func_80196AD8 | 0x180 | yes | -O2 -G0  | src/overlays/room_m174/RoomLib_MoveActorLocal_80196AD8.c |
| room_m0186i | func_8018F598 | 0x340 | yes | -O2 -G0  | src/overlays/room_m167/func_8018F578.c |
| room_m0186i | func_8018F8D8 | 0x4b8 | yes | -O2 -G0  | src/overlays/room_m186/func_8018F8D8.c |
| room_m0186i | func_80190048 | 0x1cc |  | -O2 -G0  | src/overlays/room_m186/func_80190048.c |
| room_m0186i | func_80191280 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m186/RoomLib_HandlerD.c |
| room_m0186i | func_80191FC0 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m186/RoomLib_HandlerE.c |
| room_m0186i | func_801929BC | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m186/RoomLib_HandlerB.c |
| room_m0186i | func_80192D50 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m186/func_80192D50.c |
| room_m0186i | func_801934D4 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m186/RoomLib_HandlerC.c |
| room_m0186i | func_80193958 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m186/func_80193958.c |
| room_m0186i | func_80193F94 | 0x180 | yes | -O2 -G0  | src/overlays/room_m186/RoomLib_MoveActorLocal_80193F94.c |
| room_m0187i | func_8018F1D4 | 0x1cc |  | -O2 -G0  | src/overlays/room_m187/func_8018F1D4.c |
| room_m0188i | func_8018F664 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/RoomLib_HandlerD.c |
| room_m0188i | func_8018FC8C | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0188i | func_801903A4 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/RoomLib_HandlerE.c |
| room_m0188i | func_80190DA0 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/RoomLib_HandlerB.c |
| room_m0188i | func_80191134 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/func_80191134.c |
| room_m0188i | func_801918B8 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/RoomLib_HandlerC.c |
| room_m0188i | func_80191D3C | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m123/func_80191D3C.c |
| room_m0188i | func_80192378 | 0x180 | yes | -O2 -G0  | src/overlays/room_m123/RoomLib_MoveActorLocal_80192378.c |
| room_m0188i | func_801930B4 | 0x24c |  | -O2 -G0  | src/overlays/room_m188/RoomEffect_TimedSixthSpawn.c |
| room_m0188i | func_80193890 | 0x33c |  | -O2 -G0  | src/overlays/room_m188/RoomEffect_RandomizedTriple.c |
| room_m0203i | func_8018F684 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m203/RoomLib_HandlerD.c |
| room_m0203i | func_8018FCAC | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0203i | func_801903C4 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m203/RoomLib_HandlerE.c |
| room_m0203i | func_80190DC0 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m203/RoomLib_HandlerB.c |
| room_m0203i | func_80191154 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m203/func_80191154.c |
| room_m0203i | func_801918D8 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m203/RoomLib_HandlerC.c |
| room_m0203i | func_80191D5C | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m203/func_80191D5C.c |
| room_m0203i | func_80192398 | 0x180 | yes | -O2 -G0  | src/overlays/room_m203/RoomLib_MoveActorLocal_80192398.c |
| room_m0203i | func_8019253C | 0x2e0 |  | -O2 -G0  | src/overlays/room_m203/func_8019253C.c |
| room_m0205i | func_8018F63C | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m205/RoomLib_HandlerD.c |
| room_m0205i | func_8018FC64 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0205i | func_8019037C | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m205/RoomLib_HandlerE.c |
| room_m0205i | func_80190D78 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m205/RoomLib_HandlerB.c |
| room_m0205i | func_8019110C | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m205/func_8019110C.c |
| room_m0205i | func_80191890 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m205/RoomLib_HandlerC.c |
| room_m0205i | func_80191D14 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m205/func_80191D14.c |
| room_m0205i | func_80192350 | 0x180 | yes | -O2 -G0  | src/overlays/room_m205/RoomLib_MoveActorLocal_80192350.c |
| room_m0231i | func_8018F690 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerD.c |
| room_m0231i | func_8018FCB8 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0231i | func_801903D0 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerE.c |
| room_m0231i | func_80190DCC | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerB.c |
| room_m0231i | func_80191160 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/func_80191160.c |
| room_m0231i | func_801918E4 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerC.c |
| room_m0231i | func_80191D68 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/func_80191D68.c |
| room_m0231i | func_801923A4 | 0x180 | yes | -O2 -G0  | src/overlays/room_m231/RoomLib_MoveActorLocal_801923A4.c |
| room_m0231i | func_80192C8C | 0x2e0 |  | -O2 -G0  | src/overlays/room_m231/func_80192C8C.c |
| room_m0245i | func_8018F648 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/RoomLib_HandlerD.c |
| room_m0245i | func_8018FC70 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0245i | func_80190388 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/RoomLib_HandlerE.c |
| room_m0245i | func_80190D84 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/RoomLib_HandlerB.c |
| room_m0245i | func_80191118 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/func_80191118.c |
| room_m0245i | func_8019189C | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/RoomLib_HandlerC.c |
| room_m0245i | func_80191D20 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/func_80191D20.c |
| room_m0245i | func_8019235C | 0x180 | yes | -O2 -G0  | src/overlays/room_m245/RoomLib_MoveActorLocal_8019235C.c |
| room_m0256i | func_8018F6C4 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m256/RoomLib_HandlerD.c |
| room_m0256i | func_8018FCEC | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0256i | func_80190404 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m256/RoomLib_HandlerE.c |
| room_m0256i | func_80190E00 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m256/RoomLib_HandlerB.c |
| room_m0256i | func_80191194 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m256/func_80191194.c |
| room_m0256i | func_80191918 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m256/RoomLib_HandlerC.c |
| room_m0256i | func_80191D9C | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m256/func_80191D9C.c |
| room_m0256i | func_801923D8 | 0x180 | yes | -O2 -G0  | src/overlays/room_m256/RoomLib_MoveActorLocal_801923D8.c |
| room_m0256i | func_80195320 | 0x20c |  | -O2 -G0  | src/overlays/room_m256/func_80195320.c |
| room_m0256i | func_8019552C | 0x1fc | yes | -O2 -G0  | src/overlays/room_m256/RoomEffect_PulsingSpriteCallback.c |
| room_m0263i | func_8018F654 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m263/RoomLib_HandlerD.c |
| room_m0263i | func_8018FC7C | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0263i | func_80190394 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m263/RoomLib_HandlerE.c |
| room_m0263i | func_80190D90 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m263/RoomLib_HandlerB.c |
| room_m0263i | func_80191124 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m263/func_80191124.c |
| room_m0263i | func_801918A8 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m263/RoomLib_HandlerC.c |
| room_m0263i | func_80191D2C | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m263/func_80191D2C.c |
| room_m0263i | func_80192368 | 0x180 | yes | -O2 -G0  | src/overlays/room_m263/RoomLib_MoveActorLocal_80192368.c |
| room_m0263i | func_80193180 | 0x39c | yes | -O2 -G0  | src/overlays/room_m263/func_80193180.c |
| room_m0269i | func_8018F18C | 0x1ac |  | -O2 -G0  | src/overlays/room_m269/RoomEffect_StartMotionOnProximity.c |
| room_m0269i | func_8018F338 | 0x29c |  | -O2 -G0  | src/overlays/room_m269/RoomEffect_AdvanceMotion.c |
| room_m0269i | func_8018F800 | 0x264 |  | -O2 -G0  | src/overlays/room_m269/RoomEffect_RenderMotionSprite.c |
| room_m0273i | func_8018F64C | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m273/RoomLib_HandlerD.c |
| room_m0273i | func_8018FC74 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0273i | func_8019038C | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m273/RoomLib_HandlerE.c |
| room_m0273i | func_80190D88 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m273/RoomLib_HandlerB.c |
| room_m0273i | func_8019111C | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m273/func_8019111C.c |
| room_m0273i | func_801918A0 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m273/RoomLib_HandlerC.c |
| room_m0273i | func_80191D24 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m273/func_80191D24.c |
| room_m0273i | func_80192360 | 0x180 | yes | -O2 -G0  | src/overlays/room_m273/RoomLib_MoveActorLocal_80192360.c |
| room_m0273i | func_80193870 | 0x144 |  | -O2 -G0  | src/overlays/room_m273/RoomEffect_PointerPaletteSprite.c |
| room_m0273i | func_801947CC | 0x220 |  | -O2 -G0  | src/overlays/room_m273/RoomEffect_ThresholdPalette.c |
| room_m0273i | func_801960F4 | 0x254 |  | -O2 -G0  | src/overlays/room_m273/RoomEffect_PeriodicDualPools.c |
| room_m0273i | func_801969D8 | 0x554 | yes | -O2 -G0  | src/overlays/room_m273/RoomEffect_DirectedRings.c |
| room_m0273i | func_801996C4 | 0x28c |  | -O2 -G0  | src/overlays/room_m273/RoomEffect_TemplateBurst.c |
| room_m0273i | func_8019A398 | 0x134 |  | -O2 -G0 -fno-schedule-insns  | src/overlays/room_m273/func_8019A398.c |
| room_m0318i | func_8018F6A8 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m318/RoomLib_HandlerD.c |
| room_m0318i | func_8018FCD0 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0318i | func_801903E8 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m318/RoomLib_HandlerE.c |
| room_m0318i | func_80190DE4 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m318/RoomLib_HandlerB.c |
| room_m0318i | func_80191178 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m318/func_80191178.c |
| room_m0318i | func_801918FC | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m318/RoomLib_HandlerC.c |
| room_m0318i | func_80191D80 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m318/func_80191D80.c |
| room_m0318i | func_801923BC | 0x180 | yes | -O2 -G0  | src/overlays/room_m318/RoomLib_MoveActorLocal_801923BC.c |
| room_m0318i | func_80193B60 | 0x228 |  | -O2 -G0  | src/overlays/room_m318/RoomEffect_SparkEmitter.c |
| room_m0318i | func_80193D88 | 0x23c |  | -O2 -G0  | src/overlays/room_m318/func_80193D88.c |
| room_m0318i | func_80196C48 | 0x204 |  | -O2 -G0  | src/overlays/room_m318/RoomEffect_DriftingPalettePulse.c |
| room_m0350i | func_8018F650 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m350/RoomLib_HandlerD.c |
| room_m0350i | func_8018FC78 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0350i | func_80190390 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m350/RoomLib_HandlerE.c |
| room_m0350i | func_80190D8C | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m350/RoomLib_HandlerB.c |
| room_m0350i | func_80191120 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m350/func_80191120.c |
| room_m0350i | func_801918A4 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m350/RoomLib_HandlerC.c |
| room_m0350i | func_80191D28 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m350/func_80191D28.c |
| room_m0350i | func_80192364 | 0x180 | yes | -O2 -G0  | src/overlays/room_m350/RoomLib_MoveActorLocal_80192364.c |
| room_m0350i | func_80192508 | 0x29c |  | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m350/RoomEffect_PulsedPairSpawner.c |
| room_m0350i | func_80193D6C | 0x2e0 | yes | -O2 -G0  | src/overlays/room_m350/RoomEffect_PairedTurnController.c |
| room_m0350i | func_801947BC | 0x540 | yes | -O2 -G0  | src/overlays/room_m350/RoomEffect_HomingTrailController.c |
| room_m0350i | func_801958DC | 0x288 | yes | -O2 -G0  | src/overlays/room_m350/RoomEffect_SequenceController.c |
| room_m0350i | func_80195CE0 | 0x3e4 |  | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m350/RoomEffect_FadingPairEmitter.c |
| room_m0350i | func_801960C4 | 0x814 | yes | -O2 -G0  | src/overlays/room_m350/RoomEffect_TurningAreaController.c |
| room_m0350i | func_80197364 | 0x230 |  | -O2 -G0  | src/overlays/room_m350/RoomEffect_AimedImpactController.c |
| room_m0350i | func_80197DA8 | 0x428 |  | -O2 -G0  | src/overlays/room_m350/RoomEffect_AnimationEventController.c |
| room_m0350i | func_801981D0 | 0x230 | yes | -O2 -G0  | src/overlays/room_m350/RoomEffect_TransformedFlare.c |
| room_m0350i | func_80198400 | 0x460 | yes | -O2 -G0  | src/overlays/room_m350/RoomEffect_SweptAreaEmitter.c |
| room_m0350i | func_80198860 | 0x25c |  | -O2 -G0  | src/overlays/room_m350/RoomEffect_AttachedCloud.c |
| room_m0350i | func_80198ABC | 0x23c |  | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m350/RoomEffect_AttachedCloudSpawner.c |
| room_m0350i | func_80198CF8 | 0x450 |  | -O2 -G0  | src/overlays/room_m350/RoomEffect_CloudController.c |
| room_m0350i | func_8019988C | 0x280 |  | -O2 -G0  | src/overlays/room_m350/RoomEffect_ModelTransformEmitter.c |
| room_m0358i | func_8018F690 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerD.c |
| room_m0358i | func_8018FCB8 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0358i | func_801903D0 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerE.c |
| room_m0358i | func_80190DCC | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerB.c |
| room_m0358i | func_80191160 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/func_80191160.c |
| room_m0358i | func_801918E4 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerC.c |
| room_m0358i | func_80191D68 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/func_80191D68.c |
| room_m0358i | func_801923A4 | 0x180 | yes | -O2 -G0  | src/overlays/room_m231/RoomLib_MoveActorLocal_801923A4.c |
| room_m0391i | func_8018F574 | 0x4a0 | yes | -O2 -G0  | src/overlays/room_m065/func_8018F388.c |
| room_m0391i | func_80190648 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m391/RoomLib_HandlerD.c |
| room_m0391i | func_80190C70 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0391i | func_80191388 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m391/RoomLib_HandlerE.c |
| room_m0391i | func_80191D84 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m391/RoomLib_HandlerB.c |
| room_m0391i | func_80192118 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m391/func_80192118.c |
| room_m0391i | func_8019289C | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m391/RoomLib_HandlerC.c |
| room_m0391i | func_80192D20 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m391/func_80192D20.c |
| room_m0391i | func_8019335C | 0x180 | yes | -O2 -G0  | src/overlays/room_m391/RoomLib_MoveActorLocal_8019335C.c |
| room_m0391i | func_80193500 | 0x39c | yes | -O2 -G0  | src/overlays/room_m391/func_80193500.c |
| room_m0392i | func_80190798 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m392/RoomLib_HandlerD.c |
| room_m0392i | func_80190DC0 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0392i | func_801914D8 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m392/RoomLib_HandlerE.c |
| room_m0392i | func_80191ED4 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m392/RoomLib_HandlerB.c |
| room_m0392i | func_80192268 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m392/func_80192268.c |
| room_m0392i | func_801929EC | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m392/RoomLib_HandlerC.c |
| room_m0392i | func_80192E70 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m392/func_80192E70.c |
| room_m0392i | func_801934AC | 0x180 | yes | -O2 -G0  | src/overlays/room_m392/RoomLib_MoveActorLocal_801934AC.c |
| room_m0392i | func_80193650 | 0x39c | yes | -O2 -G0  | src/overlays/room_m392/func_80193650.c |
| room_m0393i | func_8018F598 | 0x4a0 | yes | -O2 -G0  | src/overlays/room_m065/func_8018F388.c |
| room_m0393i | func_80191788 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m393/RoomLib_HandlerD.c |
| room_m0393i | func_80191DB0 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0393i | func_801924C8 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m393/RoomLib_HandlerE.c |
| room_m0393i | func_80192EC4 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m393/RoomLib_HandlerB.c |
| room_m0393i | func_80193258 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m393/func_80193258.c |
| room_m0393i | func_801939DC | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m393/RoomLib_HandlerC.c |
| room_m0393i | func_80193E60 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m393/func_80193E60.c |
| room_m0393i | func_8019449C | 0x180 | yes | -O2 -G0  | src/overlays/room_m393/RoomLib_MoveActorLocal_8019449C.c |
| room_m0399i | func_8018F568 | 0x4a0 | yes | -O2 -G0  | src/overlays/room_m065/func_8018F388.c |
| room_m0399i | func_8019063C | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m399/RoomLib_HandlerD.c |
| room_m0399i | func_80190C64 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0399i | func_8019137C | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m399/RoomLib_HandlerE.c |
| room_m0399i | func_80191D78 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m399/RoomLib_HandlerB.c |
| room_m0399i | func_8019210C | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m399/func_8019210C.c |
| room_m0399i | func_80192890 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m399/RoomLib_HandlerC.c |
| room_m0399i | func_80192D14 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m399/func_80192D14.c |
| room_m0399i | func_80193350 | 0x180 | yes | -O2 -G0  | src/overlays/room_m399/RoomLib_MoveActorLocal_80193350.c |
| room_m0399i | func_80193CE4 | 0x198 | yes | -O2 -G0  | src/overlays/room_m399/RoomLib_UpdateTimedRender_80193CE4.c |
| room_m0401i | func_8018F5A0 | 0x4a0 | yes | -O2 -G0  | src/overlays/room_m065/func_8018F388.c |
| room_m0401i | func_80191790 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m401/RoomLib_HandlerD.c |
| room_m0401i | func_80191DB8 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0401i | func_801924D0 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m401/RoomLib_HandlerE.c |
| room_m0401i | func_80192ECC | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m401/RoomLib_HandlerB.c |
| room_m0401i | func_80193260 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m401/func_80193260.c |
| room_m0401i | func_801939E4 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m401/RoomLib_HandlerC.c |
| room_m0401i | func_80193E68 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m401/func_80193E68.c |
| room_m0401i | func_801944A4 | 0x180 | yes | -O2 -G0  | src/overlays/room_m401/RoomLib_MoveActorLocal_801944A4.c |
| room_m0401i | func_80194E38 | 0x198 | yes | -O2 -G0  | src/overlays/room_m401/RoomLib_UpdateTimedRender_80194E38.c |
| room_m0404i | func_8018F688 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m404/RoomLib_HandlerD.c |
| room_m0404i | func_8018FCB0 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0404i | func_801903C8 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m404/RoomLib_HandlerE.c |
| room_m0404i | func_80190DC4 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m404/RoomLib_HandlerB.c |
| room_m0404i | func_80191158 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m404/func_80191158.c |
| room_m0404i | func_801918DC | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m404/RoomLib_HandlerC.c |
| room_m0404i | func_80191D60 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m404/func_80191D60.c |
| room_m0404i | func_8019239C | 0x180 | yes | -O2 -G0  | src/overlays/room_m404/RoomLib_MoveActorLocal_8019239C.c |
| room_m0404i | func_80193368 | 0x278 |  | -O2 -G0  | src/overlays/room_m404/RoomEffect_M404ParticleController.c |
| room_m0412i | func_8018F7A0 | 0x278 | yes | -O2 -G0  | src/overlays/room_m141/func_8018F5FC.c |
| room_m0412i | func_801900C0 | 0x190 | yes | -O2 -G0  | src/overlays/scene_e02/func_801900C0.c |
| room_m0412i | func_80190B54 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e02/RoomLib_HandlerD.c |
| room_m0412i | func_8019117C | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0412i | func_80191894 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e02/RoomLib_HandlerE.c |
| room_m0412i | func_80192290 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e02/func_80192290.c |
| room_m0412i | func_80192624 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e02/RoomLib_AdvanceArcToTarget_80192624.c |
| room_m0412i | func_8019322C | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e02/RoomLib_AdvanceArcToTargetY_8019322C.c |
| room_m0412i | func_80193868 | 0x180 | yes | -O2 -G0  | src/overlays/scene_e02/RoomLib_MoveActorLocal_80193868.c |
| room_m0412i | func_801950D4 | 0x2f4 |  | -O2 -G0  | src/overlays/scene_e02/RoomLib_RunSixteen_801950D4.c |
| room_m0413i | func_8018F648 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/RoomLib_HandlerD.c |
| room_m0413i | func_8018FC70 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0413i | func_80190388 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/RoomLib_HandlerE.c |
| room_m0413i | func_80190D84 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/RoomLib_HandlerB.c |
| room_m0413i | func_80191118 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/func_80191118.c |
| room_m0413i | func_8019189C | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/RoomLib_HandlerC.c |
| room_m0413i | func_80191D20 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m245/func_80191D20.c |
| room_m0413i | func_8019235C | 0x180 | yes | -O2 -G0  | src/overlays/room_m245/RoomLib_MoveActorLocal_8019235C.c |
| room_m0414i | func_8018F5C8 | 0x278 | yes | -O2 -G0  | src/overlays/room_m141/func_8018F5FC.c |
| room_m0414i | func_8018FEE8 | 0x190 | yes | -O2 -G0  | src/overlays/scene_e04/func_8018FEE8.c |
| room_m0414i | func_80191BE0 | 0x2f4 |  | -O2 -G0  | src/overlays/scene_e04/RoomLib_RunSixteenPointState_80191BE0.c |
| room_m0418i | func_8018F958 | 0x24c |  | -O2 -G0  | src/overlays/scene_e08/Scene_InitParticleCluster_8018F958.c |
| room_m0418i | func_80192904 | 0x374 |  | -O2 -G0  | src/overlays/scene_e08/Scene_UpdateMovingRadius_80192904.c |
| room_m0418i | func_80193C68 | 0x308 |  | -O2 -G0  | src/overlays/scene_e08/Scene_InitEffectPalettes.c |
| room_m0418i | func_80194498 | 0x23c |  | -O2 -G0  | src/overlays/scene_e08/Scene_DrawLayeredSprite.c |
| room_m0418i | func_801946D4 | 0x2b4 |  | -O2 -G0  | src/overlays/scene_e08/Scene_UpdateLayeredSprite.c |
| room_m0418i | func_801957BC | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e08/RoomLib_HandlerD.c |
| room_m0418i | func_80195DE4 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0418i | func_801964FC | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e08/RoomLib_HandlerE.c |
| room_m0418i | func_80196EF8 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e08/func_80196EF8.c |
| room_m0418i | func_8019728C | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e08/RoomLib_AdvanceArcToTarget_8019728C.c |
| room_m0418i | func_80197A10 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e08/RoomLib_HandlerC.c |
| room_m0418i | func_80197E94 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e08/RoomLib_AdvanceArcToTargetY_80197E94.c |
| room_m0418i | func_801984D0 | 0x180 | yes | -O2 -G0  | src/overlays/scene_e08/RoomLib_MoveActorLocal_801984D0.c |
| room_m0419i | func_801910AC | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e09/RoomLib_HandlerD.c |
| room_m0419i | func_801916D4 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0419i | func_80191DEC | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e09/RoomLib_HandlerE.c |
| room_m0419i | func_801927E8 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e09/func_801927E8.c |
| room_m0419i | func_80192B7C | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e09/RoomLib_AdvanceArcToTarget_80192B7C.c |
| room_m0419i | func_80193300 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e09/RoomLib_HandlerC.c |
| room_m0419i | func_80193784 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e09/RoomLib_AdvanceArcToTargetY_80193784.c |
| room_m0419i | func_80193DC0 | 0x180 | yes | -O2 -G0  | src/overlays/scene_e09/RoomLib_MoveActorLocal_80193DC0.c |
| room_m0421i | func_8018F554 | 0x300 | yes | -O2 -G0  | src/overlays/scene_e11/func_8018F55C.c |
| room_m0421i | func_8018FD28 | 0x2cc | yes | -O2 -G0  | src/overlays/scene_e11/func_8018FD30.c |
| room_m0424i | func_8018F510 | 0x300 | yes | -O2 -G0  | src/overlays/scene_e14/func_8018F518.c |
| room_m0424i | func_8018FCE4 | 0x2cc | yes | -O2 -G0  | src/overlays/scene_e14/func_8018FCEC.c |
| room_m0428i | func_8018F630 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/RoomLib_HandlerD.c |
| room_m0428i | func_8018FC58 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0428i | func_80190370 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/RoomLib_HandlerE.c |
| room_m0428i | func_80190D6C | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/RoomLib_HandlerB.c |
| room_m0428i | func_80191100 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/func_80191100.c |
| room_m0428i | func_80191884 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/RoomLib_HandlerC.c |
| room_m0428i | func_80191D08 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m022/func_80191D08.c |
| room_m0428i | func_80192344 | 0x180 | yes | -O2 -G0  | src/overlays/room_m022/RoomLib_MoveActorLocal_80192344.c |
| room_m0429i | func_8018F6D4 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e19/RoomLib_HandlerD.c |
| room_m0429i | func_8018FCFC | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0429i | func_80190414 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e19/RoomLib_HandlerE.c |
| room_m0429i | func_80190E10 | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e19/func_80190E10.c |
| room_m0429i | func_801911A4 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e19/RoomLib_AdvanceArcToTarget_801911A4.c |
| room_m0429i | func_80191928 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e19/RoomLib_HandlerC.c |
| room_m0429i | func_80191DAC | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/scene_e19/RoomLib_AdvanceArcToTargetY_80191DAC.c |
| room_m0429i | func_801923E8 | 0x180 | yes | -O2 -G0  | src/overlays/scene_e19/RoomLib_MoveActorLocal_801923E8.c |
| room_m0432i | func_8018F690 | 0x548 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerD.c |
| room_m0432i | func_8018FCB8 | 0x240 | yes | -O2 -G0  | src/overlays/room_m014/RoomLib_TransformTargetOffset_8018FC68.c |
| room_m0432i | func_801903D0 | 0x58c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerE.c |
| room_m0432i | func_80190DCC | 0x38c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerB.c |
| room_m0432i | func_80191160 | 0x3a0 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/func_80191160.c |
| room_m0432i | func_801918E4 | 0x47c | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/RoomLib_HandlerC.c |
| room_m0432i | func_80191D68 | 0x388 | yes | -O2 -G0 {'MASPSX_EXPAND_DIV': '1'} | src/overlays/room_m231/func_80191D68.c |
| room_m0432i | func_801923A4 | 0x180 | yes | -O2 -G0  | src/overlays/room_m231/RoomLib_MoveActorLocal_801923A4.c |
| room_m0432i | func_801981B0 | 0x198 |  | -O2 -G0  | src/overlays/scene_e22/func_801981B0.c |
| disc1 | func_80014BA0 | 0x200 | | era_o2_g8 | src/main/main/Camera_StepAngleFade.c |

## EXE verify lines

* func_80014BA0: `disc1_preflight: PASS (deep, 2143 c / 148 asm / 2 rodata)`
* func_80014BA0: `LEAF_STRONG=EXACT func_80014BA0 span=0x200 vram=0x80014BA0 compiled=0x200 failures=[none]`
* func_80014BA0: `era_link_check -O2 -G8: LINK_EXACT; -O2 -G0: not exact (profile load-bearing)`
