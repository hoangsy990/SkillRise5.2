# Grow Lancer isolated server QA

The QA stack lives entirely below
`ExGameServer\Tests\GrowLancerBuild\ServerStackSmoke`. It was seeded by a
read-only copy of `D:\RISE-CrossPlatform\Mu Server`; the current Ex603
Grow Lancer GameServer build is staged under the unique name
`GameServer-GrowLancer-S21-QA.exe`. No executable or Data file in the source
server tree is modified or launched in place.

`STACK_MANIFEST.json` pins all four service hashes and requires
`4.MuServer\Sub-1\Data\CashShop\CashShopPackage.txt`. From the GameServer
working directory this resolves as `..\Data\CashShop\CashShopPackage.txt`,
preventing the working-directory defect observed in the concurrent Slayer
stack.

Run the static check from the worktree root:

```powershell
python tools\grow_lancer\verify_isolated_server_stack.py
```

Start only when no other MU server stack is active:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\grow_lancer\start_isolated_server_stack.ps1
```

The launcher verifies the manifest and refuses any foreign ConnectServer,
DataServer, JoinServer or GameServer process to avoid port collisions. Each
component is started with its own directory as `WorkingDirectory`.

Stop only this stack with:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools\grow_lancer\stop_isolated_server_stack.ps1
```

The stop script matches canonical executable paths below the isolated stack;
it never terminates production or another task's processes.

Current status: prepared and statically verified. Launch was correctly refused
while the Slayer stack owned the ports. This is not database, login, movement,
cast, damage or persistence acceptance.
