#pragma once
#include <Arduino.h>

static const char INDEX_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#0b0f14">
<title>KazaKhan's Fibonacci Clock</title>
<link rel="icon" type="image/png" href="data:image/png;base64,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">
<style>
:root{
  --bg:#0b0f14;--card:#141b24;--card2:#1a2431;--line:#26313f;--txt:#e8eef6;--dim:#8fa0b4;
  --accent:#4cc2ff;--good:#3ddc84;--bad:#ff5d5d;--r:16px;
}
*{box-sizing:border-box}
html,body{margin:0;padding:0}
body{background:radial-gradient(1100px 560px at 50% -220px,#13212f 0,var(--bg) 62%);color:var(--txt);
  font-family:system-ui,-apple-system,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;line-height:1.45;
  min-height:100vh;-webkit-font-smoothing:antialiased;-webkit-tap-highlight-color:transparent}
.wrap{max-width:940px;margin:0 auto;padding:20px 16px 64px}
header{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:18px}
.brand{display:flex;align-items:center;gap:12px;min-width:0}
.logo{height:44px;width:auto;border-radius:8px;display:block;flex:0 0 auto}
h1{font-size:20px;margin:0;font-weight:650;letter-spacing:.2px}
.sub{color:var(--dim);font-size:12px;margin-top:2px;word-break:break-all}
.pill{display:inline-flex;align-items:center;gap:8px;background:var(--card);border:1px solid var(--line);
  padding:7px 13px;border-radius:999px;font-size:12.5px;color:var(--dim);white-space:nowrap}
.dot{width:8px;height:8px;border-radius:50%;background:var(--bad);box-shadow:0 0 10px currentColor;transition:.3s}
.dot.ok{background:var(--good)}
.grid{display:grid;grid-template-columns:1fr;gap:16px;max-width:760px;margin:0 auto}
.card{background:linear-gradient(180deg,var(--card),var(--card2));border:1px solid var(--line);
  border-radius:var(--r);padding:16px;box-shadow:0 10px 30px rgba(0,0,0,.25)}
.card h2{font-size:12px;text-transform:uppercase;letter-spacing:1px;color:var(--dim);margin:16px 0 12px;font-weight:650}
.acc{padding:0;overflow:hidden}
.acc>summary{list-style:none;cursor:pointer;padding:16px;display:flex;align-items:center;justify-content:space-between;
  font-size:12px;text-transform:uppercase;letter-spacing:1px;color:var(--dim);font-weight:650;user-select:none}
.acc>summary::-webkit-details-marker{display:none}
.acc>summary::after{content:"";width:8px;height:8px;margin-right:3px;
  border-right:2px solid currentColor;border-bottom:2px solid currentColor;
  transform:rotate(45deg);transition:transform .2s}
.acc[open]>summary::after{transform:rotate(-135deg)}
.acc>summary:hover{color:var(--txt)}
.acc-body{padding:0 16px 16px}
.acc-body>h2:first-child{margin-top:0}
.clock{display:flex;flex-direction:column;align-items:center;gap:6px;padding:22px 16px}
#face{width:100%;height:auto;max-width:560px;display:block;transition:opacity .4s}
#face rect{transition:fill .3s}
#face text{font:600 0.5px system-ui,-apple-system,"Segoe UI",Roboto,sans-serif;text-anchor:middle;dominant-baseline:central;pointer-events:none}
.time{font-variant-numeric:tabular-nums;font-size:42px;font-weight:750;letter-spacing:1.5px;line-height:1}
.date{color:var(--dim);font-size:13px}
.rep{color:var(--dim);font-size:12.5px;text-align:center;min-height:18px}
.row{display:flex;align-items:center;justify-content:space-between;gap:14px;padding:7px 0}
.row>span{font-size:14px}
input[type=range]{-webkit-appearance:none;appearance:none;height:6px;border-radius:999px;background:var(--line);
  width:58%;outline:none}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:20px;height:20px;border-radius:50%;
  background:var(--accent);cursor:pointer;border:3px solid #0b0f14;box-shadow:0 0 0 1px var(--line)}
input[type=range]::-moz-range-thumb{width:16px;height:16px;border-radius:50%;background:var(--accent);
  cursor:pointer;border:3px solid #0b0f14}
.switch{position:relative;display:inline-block;width:52px;height:30px;flex:0 0 auto}
.switch input{opacity:0;width:0;height:0}
.switch span{position:absolute;inset:0;background:var(--line);border-radius:999px;transition:.25s;cursor:pointer}
.switch span:before{content:"";position:absolute;width:22px;height:22px;left:4px;top:4px;border-radius:50%;
  background:#fff;transition:.25s}
.switch input:checked+span{background:var(--accent)}
.switch input:checked+span:before{transform:translateX(22px)}
.themes{display:grid;grid-template-columns:repeat(auto-fill,minmax(96px,1fr));gap:9px}
.theme{border:1px solid var(--line);border-radius:12px;padding:8px;background:#0f151d;cursor:pointer;
  text-align:left;color:var(--txt);font-size:12px;transition:.18s}
.theme:hover{border-color:#3a4a5e;transform:translateY(-1px)}
.theme.sel{border-color:var(--accent);box-shadow:0 0 0 1px var(--accent) inset,0 0 14px rgba(76,194,255,.25)}
.bar{display:flex;height:10px;border-radius:6px;overflow:hidden;margin-bottom:6px}
.bar i{flex:1}
.field{display:block;font-size:12.5px;color:var(--dim);margin:10px 0}
.field input,.field select{width:100%;margin-top:5px;background:#0f151d;border:1px solid var(--line);color:var(--txt);
  border-radius:10px;padding:10px 11px;font-size:14px;outline:none}
.field input:focus,.field select:focus{border-color:var(--accent)}
.segs{display:grid;grid-template-columns:repeat(5,1fr);gap:8px}
.segs label{display:block;font-size:11px;color:var(--dim);text-align:center}
.segs input{width:100%;margin-top:4px;text-align:center;background:#0f151d;border:1px solid var(--line);
  color:var(--txt);border-radius:10px;padding:9px 2px;font-size:15px;outline:none;font-variant-numeric:tabular-nums}
.segs input:focus{border-color:var(--accent)}
.btns{display:flex;gap:9px;flex-wrap:wrap;margin-top:12px}
button{font:inherit;font-size:13.5px;padding:10px 14px;border-radius:10px;border:1px solid var(--line);
  background:#182231;color:var(--txt);cursor:pointer;transition:.16s}
button:hover{border-color:#3a4a5e;background:#1e2a3a}
button.primary{background:var(--accent);border-color:var(--accent);color:#04121c;font-weight:650}
button.primary:hover{filter:brightness(1.08)}
button.danger{color:var(--bad);border-color:#4a2a2e}
button.danger:hover{background:#2a1a1d}
.stats{display:grid;grid-template-columns:1fr 1fr;gap:8px 14px;font-size:13px}
.stats div{display:flex;justify-content:space-between;gap:8px;border-bottom:1px dashed var(--line);padding:5px 0}
.stats b{font-weight:600;color:var(--dim);font-weight:500}
.stats span{font-variant-numeric:tabular-nums}
#fw input[type=file]{width:100%;font-size:12.5px;color:var(--dim);margin-top:4px}
#toast{position:fixed;left:50%;bottom:22px;transform:translate(-50%,20px);background:#1e2a3a;border:1px solid var(--line);
  color:var(--txt);padding:11px 18px;border-radius:12px;font-size:13.5px;opacity:0;pointer-events:none;transition:.28s;
  box-shadow:0 12px 30px rgba(0,0,0,.4);z-index:9}
#toast.show{opacity:1;transform:translate(-50%,0)}
.link{background:none;border:0;color:var(--accent);font:inherit;font-size:12.5px;cursor:pointer;padding:4px 0;
  text-decoration:underline dotted;text-underline-offset:3px}
.link:hover{color:#7fd4ff}
dialog.howto{border:1px solid var(--line);border-radius:var(--r);color:var(--txt);position:relative;
  background:linear-gradient(180deg,var(--card),var(--card2));max-width:440px;width:calc(100% - 32px);
  padding:20px;box-shadow:0 24px 70px rgba(0,0,0,.6)}
dialog.howto::backdrop{background:rgba(4,8,12,.62);backdrop-filter:blur(2px)}
dialog.howto h3{margin:0 0 10px;font-size:15px;letter-spacing:.2px}
dialog.howto p{margin:8px 0;font-size:13.5px;color:var(--dim)}
dialog.howto b{color:var(--txt)}
dialog.howto ul{margin:8px 0;padding-left:18px;font-size:13.5px;color:var(--dim)}
dialog.howto li{margin:5px 0}
.howto-x{position:absolute;top:6px;right:10px;background:none;border:0;color:var(--dim);font-size:24px;line-height:1;cursor:pointer}
.howto-x:hover{color:var(--txt)}
.legend{display:grid;grid-template-columns:1fr 1fr;gap:9px 14px;margin:12px 0}
.lg{display:flex;align-items:center;gap:8px;font-size:13px}
.sw{width:16px;height:16px;border-radius:5px;border:1px solid rgba(255,255,255,.28);flex:0 0 auto}
footer{text-align:center;color:var(--dim);font-size:11.5px;margin-top:22px}
</style>
</head>
<body>
<div class="wrap">
  <header>
    <div class="brand">
      <img class="logo" alt="" src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEAAAAAwCAIAAAAuKetIAAAW40lEQVR42q1aaVhT19be5+RkJiQgGAyzFRVBEUSZVHBAxX7VOrT6AFZabW2xttbW2moR8Wm1CtTHAW9FBW+L2goOWERF5RJUUGQIMhRQxiBhMEhISHKGnP392JrLdep97r3710lyzj57rf2u9b5r7QAwbOA4ThAEupbL5WlpafDZaG5uLr1TOmnSpDFjxtTV1QEAdu3apdPpCgoKWJaladryXwySJCGEp06dAgBwuVwAwNy5c7dt2xocHNTV1cXQNIRw1apVAQEB58+f/+yzz/bv32+xWFiWhRACAACHw8EwzGqGk5NTQkLCwMAAhPDhgwepqalLly7dvHmzQjEKAPDpp5/OnTv343Xr+vv7e3t7W1panq2e+Y8NYBiGZdnBwUE3Nze0HoIgjh8/vnnz5tWrVz958sRoNAYFTROLxffu3SsvL3/rrf+DEObn5+fk5HBwHGdZFgBAEMTEiRNTUlJ++eWXGTOm5+TkfBL/SV1dXXh4eGho6HvvrTpw4KDBYKipqeHg+OUrV0iSNJlMbW1t7u7uAIDhLviLAf85MAxDjmQYRiQS6XQDSqWSz+dRFG1nJ1u+/B0IYWlpqaOj486dO81mc2DgFD6fn5l5wtfXt6KyorS0FAMALF++3N/fXyqVrly5cnBw8OcjPz9oeuDt7e3t7T158uRffv0lLDRs6tSpY8eONZlMLMtmZmbGxcVRFHW/ujogIADDcZZlMQyDELIsi+P4a4xBi8ZxHH20MAyHIKxGNbc0+/r4MgwDIZwwYcKbb765du3a3NxcpbJo3rz59fX1RUVF2xO3y0fK3xg9WiQWOzo6glmzZplMptDQEDc3t4hZEYsWLZozZ/aVK1e+/HIThPDkyaw33nhDWVRkMpmCgqYBACQSm0ePHun1epVKtWbNmqGhIYZhaJqmaRr5EkHiVVBBC+3p6amqqmppbYEQ3rhxvbS0FCEWQjh5sh8AgCA4AACxWBwdHZ2UtEMikbS3t6ekpKhUKrPZfPr0aQhhVlZWYmIiHhISotFoWlpak5OTXZ1d4uLinJ1dWJb9s/7PS5fyQkPDCgsLDx465OHhrlKpAADBwSEKhSI3N/fEiRPHjh3j8bgo9AmCoGn69G+nBwYGOBzO0/B6DjgA9Pb2RkREuLm5+fv7T/SdOHPmzLfeWhQSEuLr67twYVR5ebmHhycAgMMhMAxbsWJFVFSUSCT29/ffvn37b7+dVqlUGo2mqKios7NzrJfXyJEjcZZl+3p7v978dVVVVeyqVWq1WiqV1tTU2NnZtbS0xsXFLYhacO3atdWr4wiCCwDQPn5sNpurqqoCAwMBAATBNRqNg4ODpaWlgYGBsTGxtbW1er0erXi4ARaLheBwqqurlUqlQqEYMWLE0NDQzZs3jUajSCTs7Oy8fPnK1KlT6+rqOBwORVEQwuvXr//xxx8ODg6ffvrpuXPnJkzwIUlSoVCkpv7k5OQ0ddo0lUpFRISHdz56NHXa1MuXLz98+EClqiq/Vx45b152dvbvZ84wDAMAmDdvnl4/aDabAQAjHBxIkpRIJDExMXV1dTweLzY2VqPR0DTd3d09atSopKSkX3/9VSQSMQxDEASKB5ZlORxOU1OTRqNxd3cvKysLCwtDIAkNDTUajW+++eb9+/fT0tKioqJqa2sLCwtxHFer1R0dHfMiIz09PXfs2LFp0ybkFx6Phzxy9+5dACHcvn37kSNHSkpK7OzspFJbq88UCsX69fH5+fl37tz57LPPZDIZACApKSknJzs+Pn7jxo1eXl6jRjkBABAqZsyYcbXg6sOHD41GozXPDM/0CQkJPB5PJpMFBwcnJSWp1equri6GYTZu3FhcXAwhVCqVS5Ys8RrrheM4jmNcLhfH8YSEBIqi9u7ZazabGxoaWJY9d+7c119/DSH84ouNgGXZxMTE4Xs9a9ashISEe/fusSzb29t76NChESNGWCkmNjb2xx9/jI6O/urLLyMjI+PiVl+4cAFCWFZWFh4e7uzsDABwcXGJjo4+duxYV1eXxWKhKIpl2dbWVrl8JJonLy9vWC6FLMsGBQVFREQYDIbk5GQ+n8/hcDAAOBwOACAsLIyiqOnTp+fk5KxZs8ZisWzbtvXq1asQwra2NgAhXLfuI3t7++nTp3/zzTdFRUVo0sLCwu9/+GHbtm1JSUlisZjD4aDpgoOD161b5+7udvDgQesKEFkajcbm5ub333/f6gt/f3+GYRCga2pqAADOzs4PHz7Mzs6OjolOTt574MCB5JSU/Pz8y5cvAwBSU1MhhIsXL0a8hOBnZ2dnMpmWLl2ampJSWVkJIbx79058fDxJkr/+8gsgSTIxMfHGjRtdXV1oNUePHvX09Fy5cuXevXuPHDkiEokQT6EREBCQkJAQHBxcWnrHYDA8evTIZDIxFma4Rw+lpc2YMQPH8cl+fih7ogSanJzc1NSkVCpf5AeFQoHj+JQpU2iaPnEic7gBYrEYQnjy5MmUlJT33luVn5/fr9UCALRabXFxMTAajVu2bJkyZcqVK1domj537hyacdy4cenp6atWrfrkk08cHBysENq8ebNhaCgrKwtCWFpSgrBrNptv3bp14MCBsWPHenl5nT9/3mQyVVZWNDY2siyLaMFKAt9//31eXl56evpzfMflct3c3FKSkzUaDZfLRf5C256fn9/c3Pz55597engIhcLa2trvv//ebDZ3dXURBoNBLpfPnj3bz8+PIIjGxkYkLro1mvb2dnt7+/7+fpPJhIgWANDZ2SkWiWJiYmiahgCEhoYWFBQkJibeuXMHACCTyVJTU8+cOePi4oLyLGuxoIViGEbTNIZh27ZtQysOCAjQGwyZGRk0TY8fPz4yMjIwMLC6utrJycnb2/v+/fs4jiPOzsrKOnnypK+vb2hoqKOjo4eHB5pk1KhRBEVRdnZ2xcXFNE0DAKZMmUIQxIcffjh58mS9Xt/Q0KDXD6IcggwwGAzI5QKBQC6XT5o0sb7+TwCAn5/fsmXLli1dOsHH54MPPgAAoBRsVQ3gWVCirUCAAQBEhIcPJ7vAwEAI4ezZs2tqanAcR6tasGABAGDt2rXoNoqijENDBJdLkiRhMplIkty5c6dcLmcYJjIy8uzZnMrKyo6OjsFB3f79+/Py8q5du26V2TKZDMMwgsMhSTIhIcHWVrpnz56xY8eGhYXp9fpr1wpMZvOkSZMwDOA456VyyGoSQ9PwBY1E07RAIPD19YUQYgDY29vPmTNn/vz5DMNotVqtVjsw8MTZ2SU7OzsoKGjkyJGgrKzs0KFDFRUVarWaZVmUMXp7e5VKJYTw4cOHbm5uaGpkw1dffgkhpCiKIslBnc4auEqlsrS0tK+vD0nLl2ohJJkszOuEN03TEEK1Wm1nZwcAOHXqFIQwKWmHRqNZs2ZNeER4cHAQTdOHDx++fPkyhBDXarUkScpkMgcHB8SXNEU5OjrOnDkzKSkpMDCwo6MDUSlylaenJ3IYweVKbG1RlqQoaubMmcHBwQ4ODpZnoLdagrAHIUSS6fVaG8NxhmFcXFymTZsGAOjt7R0YGKiqUjk5OcXExDA04+rqShCEQCBAc+IcDqe1tbWk5LZAIHj6JgBQ3OzevVsikYhEIrS/6NdRCoV10y0Wi1XJWUWoNbdwOBwej8flcjkcDrqztbW1sbER53AQeb3cAACQqkVlRmBgII/Hs7GxAQDU19dPnTpVKBQBAKS2tllZWRaLBTcajWfPnj137rxVlBMEYTAYHjx4cPbsWbVajSSQVZkNd+HwPIhgZvU9AKC7uzsxMfHdd9/V6XRcLvf27dujR48eP378x+vWoZrrdfuAYXw+H8NAWFiYSCSiKArlHHt7u9u3bxcXF3tPmPC4r48gCFyn0x09mv77778zDIPjOMoPZWVlIqFQrVZbHf9cJnltwQVQWfPRRx/t3LkTRW1eXl5+fv7ChQuFQqFUJmttbT116hSO4xaL5aVVDwBgcHAwPDwiLS3t4cOHEokNhLCysnJgQNfR0fHDDz/Y2Ng0t7Rs2bIFF4vFKlX1/v37EQysk1A0fe3aNQgh0nCo9sNx3MXF5bnk+CIIkCOSk5PPnz9/5swZiUTC5/OioqIuXbpUW1u7Z8+e7OxsPp//qgnQ46TZnLx3r5+fn1gstrWVdnS0Ozsr1sevT09P1w0MCIVCjUZz8eJFcPr06bt37ty/f3845/f29k6bNs3Gxmbfvn3Xr19/WvgDAABAOoykqH+nVLcWaAicKMPQNI3I5FVPobiPjo7u7+9va2ujaTomJiY3NxdN0tTUmJiYaDKZPDw8Ll68iPd0d/+alaXuVD9NxgDQNO3o6Dhy5EiDwRAfH4/AyrKsnb29l5cXSkrghYLrxcEwDIofxMEMTeMYhoAqFostDPOaAOjq6sIwTCaTrVu3ruBqwbJly4xGY1tb26JFb+3ff6C+vh5xhVarxY0mU1pa2tnsnKftiWdxuW3bVisRWsuI6Oho7ePHT5H+V4PH5aLKA20gjuPgWTlvsViwV4AQA4C1WOzt7VmWffLkSUBAQHl5+dDQEJfLdXV1HRwcbGtrGxoaAgDo9Xo+n0/Y29ur1WqFQoFKPvQyhmGCg0MWLJj/97+fePfdFci2xYsXx8XF/aOwEACIYfhfd09ecKz1l9f1YDCMtVgEAoFQINDpdLGrYn0m+BAEsW3rVm9vbxuxjZeXl7NCAQD4+OOP5SPl+JDBkJOTU1xcjOM4OwwYEMK0tMMlJaUYhgkEAgzDYmJi3N3d3//gA5aFf52Lng2WZa3gRg0sloXWa/gyKHJ5PADAG2PGyGRS+Ui5vb09wzA8Hu/ixVwuj7d+/frFb78NAJBIJKpqFW4ymzdu3Hjjxg0Mw6xZCIHHw8PDyckpIyPjh10/IAgdPny4paUZZYl/c+kEQXC5XC6Xy+PxeDweYj3rNQoVK6khHjx06NDt27clEklT0wORSERRJI7jmSdO/PzzkRvXrwsEAo1Gs3v37qNH0zdt2oTduXOnq6srICDAzc0N9aesOwAA6O/v37Bhw549exYuXHjjxg0Ioa2trVAoBBCCV8AAQgggtLAsCoA///yzvaO9p6fHbDLdvHmrsqKCx+ePGzcuNDR09uzZEydOtFprlXS9vb22trYFBQUrVqyYPj2suPgmErY+Pj779u2LjIy8dOmSQqH429/+JpFIiKBp09asXWtra4sMsGIDwzCGYRwdHSMiIjQajZub65P+fu8JE1Caew2EEB9zAGhqatqyZUt+fv4oxSgbsQ0GgIen5zvvviMUipqbm7Ozs5OTkxUKhZfXmA8//CgiIoKmaTStk5MT4l2KogoL/5GUlCSR2JSXl588eeru3bsVFRXR0dG1tbXR0dF9fX1EeUVFRkaG15gxc+bMGW4AAICD4xDCOXPm8Pl8Gxub4xkZKSkpT7OqxfKqHaAosru758CBA7///vuCBQtKSkpQqfTinVqttqKioq2tTSqVIt9b5T6Px+vv78cwjCCI9PR0AMDcuXPMZrNW+7ihsdFZoRCJRENDQ0qlEgwMDOReyH1a+w3vkjMMy7IDAwMURTEMU19fP2bMmL6+vvv3q5VKJdK9LxKQTqebPXvWxIkTV6xYUVdXN6zvYDGajFqttrGx8fbt2+1tbYjUnuu+vNhtR5ZHRUXt3bu3t7fn1KlTMTExfn6TIIQajcbD3Z2QSqXBIcGDg/oX0xmEUCwWI4B6e3t/++23O3bsCA4O2rVrd319PcohVrdZC/CjR4/Z29sjAWI2m7lcbl1dXUZGhk6nMxj0o0Yp3Nzc3n77bSvJIPX1L9oEQpTmUYivXLny+PHjsbGxmzZtksvlBoOhoaExJCQkIyNDIBA8VaDUX0kDmqIghAUFBVqtdvXq1Y8fP35Z4fJUBUAISZK0bpHJZHry5Iler3+x4fXSgfY8Pj4emfPdd9/dunVr9OjREMK4uDj0pbubW0lJiY2NGOtob+/p7Z0yZQr7anZEnqYpioWsUCj66quv/P39Y2JiKIp6EdwolT2ntK0fGYaBLIs95/J/fZwgiLa2tnHjxiGYRUZGFhQU+Pj4XL16tayszEyaGZoJCwuztbXt7+8nrt+4cfNmcUZGpoVliZdNit6N4zhfIOjr6xMIhEFB06qqql51qPHiylgIwbNMj+M4wF/H4pBlIYQtLS0MwyBN0NPToywqclYoFArF0qVLrXcajUYAAHj8uG949+Y5ZWixWIxGI0VROp2uq6sL4V6j0SxYMN9sNqPDAcv/cDAM8vqiRYueqz1kMtmFCxfWrVv37bffdHR0zJ8/Xy6X8/l88JeIJEmSZdnbt27l5ORACHU63datWwEANTU1ZrP5NSX8fzDQKUlDQ4ONjY0VeFKp1Nvbm8vltre35+TkYAAEBPgDAOzt7XAcx48ePfrdd9+9qjiy5hkIQG1tbVlZ2eHDaVlZvwIALly4oNfr+/v7wf9uMAyDYVhmZqbBYEBQxDBs06ZN6enpBEHcvHnzp59+AhhWWVm1YMGCjRu/GD3aE4+KigoLC0O57MUZORwO6vKp1R16vb68vFwgEKKyOi0traKiIioqqqenB8dxloX/5eotDCMQCLq7u3MvXFi6dKmtrQS5LzU19erVqwTBuX///owZMwAAQqEwMzMzISFBKpXhra2tISEhL5c0AAwODra3t1VXVwtFIg8PD5IkXV1du7q6cBzv7u7m8/lLliwpUhYh/L6q0fAypQ3Bs8Y6GhaG4fJ4Wq02Nja2obFx48aNMpnd00LKzm7y5MkGw5CHh8fQ0BDS9oWFhXl5eSRJYomJ2z3cPeLef5+mKM6wnIisHxgYqKurKyoqkkqlbm5uFRUVubm5MpmspKSEZdm9e/YO6gcfPXp0/Phx9BTiJg6Ov0bqofL6ucNpAEBHR8f8+fMaGhqDg4OLioqmTp1aW1tLEASqSAEAXl5enZ2dJpPpJYe2r4oqlHZUKtXKlSsdHBwWL17c2tq6YcMG9OyWLVv++OMPf3//RYsWxcfHl5eXPz0ueNbtommaJEl0TZIkSZLWkpckSZ1OV1NTc7fsblNTU15enqurK4IHmmfy5MnPGb98+fK0tLT58+evX7/++PHjFy9eLCgoALt27aqurn7V3wUYhkEvLi8vX7t27dmzZ5OSkgiCQAETEBAAIWxobEAvyMzM3LBhQ17eH/C1o7S0dMaM6XFxcdOnT//888+5BCEUCq0RmJKSggr/CRMmuLi4/Lh7t0ql2rdvH4/Ha2xs1GofDw0NoXkGBwfVajVx7949Hx+fSZMmvbRGgRByudxr168V3ihUKouOHTtmbW/hOF5XV9fQ0DB+/PizZ88uW7YsOTm5vr7+4MGDfn5+QUFB77zzjqurS1lZmVQqHTlSXlVVRdN0/qVL/U+e3Lt3r7GxacyYMba2tiyEJpOJw8ExDJszZ/YXX3xBkSSPzxeLxWFhYXInpzNnzqCO7bx58zo71aNHv6FQKCiKrKqsMpMkePTo0et5AMnSoaGhcePGWZuS1lFRUYFglpqaiv4iMnfu3OFnFiNGjNi1a9cIe/vXsDUqIbhcbmNjo/VIKnxY2/11w85OplKp0K69ygA04/pn6koul0dFRf30008NDQ1WxEMIt29PkEqlwUFBAoHACjMul4tsRsXk0/bEs1Yk4lr0zTfffPO0701REMKoqCjUYETd1YULF1rLN+sfVDAMAyqV6vHjxy+XEsMiAa0yJyfnt99+02g0aNO2b9+enJwMWZamaZI0o+N/iUSCDqOGi6VXtSEwDGAYJhDwz507x7IsTVMo6iCEO3bssGIVw7ClS5Zcv369qKhoz54fPT09rTz9/2h2lGqtuQGCAAAAAElFTkSuQmCC">
      <div><h1>KazaKhan's Fibonacci Clock</h1><div class="sub" id="hostline">&mdash;</div></div>
    </div>
    <div class="pill"><span class="dot" id="syncdot"></span><span id="synctext">connecting</span></div>
  </header>

  <div class="grid">
    <section class="card clock">
      <svg id="face" viewBox="0 0 8 5" aria-label="Fibonacci clock"></svg>
      <div class="time" id="bigtime">--:--</div>
      <div class="date" id="bigdate">waiting for time</div>
      <div class="rep" id="rep"></div>
      <button class="link" id="howto" type="button">How to read this?</button>
    </section>

    <details class="card acc" open>
      <summary>Display</summary>
      <div class="acc-body">
        <div class="row"><span>Power</span>
          <label class="switch"><input type="checkbox" id="on"><span></span></label>
        </div>
        <div class="row"><span>Brightness</span><input type="range" id="bright" min="0" max="255"></div>
        <h2>Theme</h2>
        <div class="themes" id="themes"></div>
      </div>
    </details>

    <details class="card acc">
      <summary>LED layout</summary>
      <div class="acc-body">
        <div class="segs" id="segs"></div>
        <div class="row" style="margin-top:10px"><span>Total LEDs</span><b id="segtotal">0</b></div>
        <div class="row"><span>Test segments on boot</span>
          <label class="switch"><input type="checkbox" id="boottest"><span></span></label>
        </div>
        <div class="btns">
          <button id="saveseg" class="primary">Save layout</button>
          <button id="testseg">Test segments</button>
        </div>
      </div>
    </details>

    <details class="card acc">
      <summary>Time</summary>
      <div class="acc-body">
        <label class="field">Timezone<select id="tz"></select></label>
        <label class="field">Set time<input type="time" id="manual"></label>
        <div class="btns"><button id="sett" class="primary">Set time</button></div>
      </div>
    </details>

    <details class="card acc">
      <summary>System</summary>
      <div class="acc-body">
        <div class="stats" id="stats"></div>
        <div class="btns">
          <button id="reboot">Reboot</button>
          <button id="wifi" class="danger">Reset WiFi</button>
        </div>
        <h2>Firmware update</h2>
        <form id="fw" action="/update" method="post" enctype="multipart/form-data">
          <input type="file" name="firmware" accept=".bin,.bin.gz">
          <div class="btns"><button class="primary" type="submit">Upload</button></div>
        </form>
      </div>
    </details>
  </div>
  <footer id="foot">KazaKhan's Fibonacci Clock</footer>
</div>
<dialog id="howtoDlg" class="howto" aria-label="How to read the clock">
  <button class="howto-x" id="howtoClose" type="button" aria-label="Close">&times;</button>
  <div id="howtoBody"></div>
</dialog>
<div id="toast"></div>

<script>
const $=s=>document.querySelector(s);
const FIB=[1,1,2,3,5];
let SEG=[1,1,3,7,12];
const NS="http://www.w3.org/2000/svg";
// Classic 8x5 Fibonacci tiling. Index = block (bit position).
const SQUARES=[
  {x:2,y:0,w:1,h:1},   // block 0 - value 1  (top-middle)
  {x:2,y:1,w:1,h:1},   // block 1 - value 1  (below it)
  {x:0,y:0,w:2,h:2},   // block 2 - value 2  (top-left)
  {x:0,y:2,w:3,h:3},   // block 3 - value 3  (bottom-left)
  {x:3,y:0,w:5,h:5},   // block 4 - value 5  (right, full height)
];
const TZS=[
  ["UTC","UTC0"],
  ["Sydney","AEST-10AEDT,M10.1.0,M4.1.0/3"],
  ["Melbourne","AEST-10AEDT,M10.1.0,M4.1.0/3"],
  ["Brisbane","AEST-10"],
  ["Adelaide","ACST-9:30ACDT,M10.1.0,M4.1.0/3"],
  ["Perth","AWST-8"],
  ["Darwin","ACST-9:30"],
  ["Hobart","AEST-10AEDT,M10.1.0,M4.1.0/3"],
  ["Auckland","NZST-12NZDT,M9.5.0,M4.1.0/3"],
  ["London","GMT0BST,M3.5.0/1,M10.5.0"],
  ["Paris / Berlin","CET-1CEST,M3.5.0,M10.5.0/3"],
  ["Madrid","CET-1CEST,M3.5.0,M10.5.0/3"],
  ["Rome","CET-1CEST,M3.5.0,M10.5.0/3"],
  ["Athens / Helsinki","EET-2EEST,M3.5.0/3,M10.5.0/4"],
  ["Moscow","MSK-3"],
  ["New York","EST5EDT,M3.2.0,M11.1.0"],
  ["Chicago","CST6CDT,M3.2.0,M11.1.0"],
  ["Denver","MST7MDT,M3.2.0,M11.1.0"],
  ["Los Angeles","PST8PDT,M3.2.0,M11.1.0"],
  ["Anchorage","AKST9AKDT,M3.2.0,M11.1.0"],
  ["Honolulu","HST10"],
  ["Tokyo","JST-9"],
  ["Seoul","KST-9"],
  ["Shanghai","CST-8"],
  ["Hong Kong","HKT-8"],
  ["Singapore","SGT-8"],
  ["Kolkata","IST-5:30"],
  ["Dubai","GST-4"],
  ["Johannesburg","SAST-2"],
  ["Sao Paulo","BRT3"],
  ["Buenos Aires","ART3"],
  ["Mexico City","CST6"],
];
let themes=[],state={},squares=[],segDirty=false,lastSeg="";

function buildFace(){
  const svg=$("#face");svg.innerHTML="";squares=[];
  SQUARES.forEach((s,bi)=>{
    const r=document.createElementNS(NS,"rect");
    r.setAttribute("x",(s.x+0.05).toFixed(2));
    r.setAttribute("y",(s.y+0.05).toFixed(2));
    r.setAttribute("width",(s.w-0.1).toFixed(2));
    r.setAttribute("height",(s.h-0.1).toFixed(2));
    r.setAttribute("rx","0.06");
    r.setAttribute("fill","#ffffff");
    r.setAttribute("stroke","#0b0f14");
    r.setAttribute("stroke-width","0.06");
    svg.appendChild(r);
    const t=document.createElementNS(NS,"text");
    t.setAttribute("x",(s.x+s.w/2).toFixed(2));
    t.setAttribute("y",(s.y+s.h/2).toFixed(2));
    t.setAttribute("fill","#0b0f14");
    t.textContent=FIB[bi];
    svg.appendChild(t);
    squares.push({el:r,label:t,b:bi});
  });
}

function labelColor(h){
  h=(h||"#ffffff").replace("#","");
  if(h.length===3)h=h[0]+h[0]+h[1]+h[1]+h[2]+h[2];
  const r=parseInt(h.substr(0,2),16),g=parseInt(h.substr(2,2),16),b=parseInt(h.substr(4,2),16);
  return (0.299*r+0.587*g+0.114*b)>150?"#0b0f14":"#f4f8fc";
}

function segInputTotal(){
  return [...document.querySelectorAll("#segs input")].reduce((a,i)=>a+(parseInt(i.value,10)||0),0);
}

function buildSegInputs(){
  const box=$("#segs");box.innerHTML="";
  const max=state.maxleds||100;
  FIB.forEach((v,i)=>{
    const l=document.createElement("label");
    l.textContent="fib "+v;
    const inp=document.createElement("input");
    inp.type="number";inp.min="1";inp.max=max;inp.dataset.i=i;
    inp.addEventListener("input",()=>{
      segDirty=true;
      $("#segtotal").textContent=segInputTotal();
    });
    l.appendChild(inp);
    box.appendChild(l);
  });
}

function buildTzSelect(){
  const sel=$("#tz");sel.innerHTML="";
  TZS.forEach(t=>{
    const o=document.createElement("option");
    o.value=t[1];o.textContent=t[0];
    sel.appendChild(o);
  });
}

function toast(m){const t=$("#toast");t.textContent=m;t.classList.add("show");
  clearTimeout(t._h);t._h=setTimeout(()=>t.classList.remove("show"),1800);}

function blockColor(bi,st){
  const hm=st.hourMask||0,mm=st.minMask||0,h=(hm>>bi)&1,m=(mm>>bi)&1,c=st.colors||{};
  if(h&&m)return c.both||"#ffffff";
  if(h)return c.hour||"#ffffff";
  if(m)return c.minute||"#ffffff";
  return c.off||"#000000";
}
function maskVals(m){const a=[];FIB.forEach((v,i)=>{if(m&(1<<i))a.push(v);});return a;}

const pad2=n=>String(n).padStart(2,"0");

function howtoHtml(){
  const st=state;
  const c=st.colors||{off:"#ffffff",hour:"#ff0a0a",minute:"#0aff0a",both:"#0a0aff"};
  const hv=maskVals(st.hourMask||0), mv=maskVals(st.minMask||0);
  const hsum=hv.reduce((a,b)=>a+b,0), msum=mv.reduce((a,b)=>a+b,0);
  const sw=(col,txt)=>'<div class="lg"><span class="sw" style="background:'+col+'"></span>'+txt+'</div>';
  const now=st.synced?(pad2(st.hour)+":"+pad2(st.minute)):"--:--";
  return "<h3>How to read this clock</h3>"+
    "<p>Each square is worth a Fibonacci number: <b>1, 1, 2, 3, 5</b>. Add the squares of each colour to read the time.</p>"+
    '<div class="legend">'+
      sw(c.hour,"Hours only")+
      sw(c.minute,"Minutes only")+
      sw(c.both,"Hours and minutes")+
      sw(c.off,"Not used")+
    "</div>"+
    "<p>Right now the clock shows <b>"+now+"</b>.</p>"+
    "<ul>"+
      "<li>Hours = "+(hv.length?hv.join(" + "):"0")+" = <b>"+hsum+"</b></li>"+
      "<li>Minutes = ("+(mv.length?mv.join(" + "):"0")+") &times; 5 = <b>"+(msum*5)+"</b></li>"+
    "</ul>"+
    "<p>Minutes are shown in 5&#8209;minute steps.</p>";
}

function render(){
  const st=state;
  if(st.seg)SEG=st.seg.map(Number);
  const segKey=st.seg?st.seg.join(","):"";
  if(!segDirty&&segKey!==lastSeg){
    document.querySelectorAll("#segs input").forEach(inp=>{
      const i=+inp.dataset.i;
      inp.value=(st.seg&&st.seg[i]!=null)?st.seg[i]:"";
    });
    lastSeg=segKey;
  }
  if(!segDirty)$("#segtotal").textContent=st.total||SEG.reduce((a,b)=>a+b,0);
  $("#boottest").checked=!!st.bootTest;
  squares.forEach(s=>{
    const col=blockColor(s.b,st);
    s.el.setAttribute("fill",col);
    s.label.setAttribute("fill",labelColor(col));
  });
  const br=(st.brightness??255)/255;
  $("#face").style.opacity=(0.28+0.72*br).toFixed(2);
  const tHv=maskVals(st.hourMask||0), tMv=maskVals(st.minMask||0);
  $("#face").title=st.synced
    ? ("Now "+pad2(st.hour)+":"+pad2(st.minute)+" \u2014 hours: "+(tHv.join("+")||"0")+
       ", minutes: ("+(tMv.join("+")||"0")+")\u00d75. Click \"How to read this?\"")
    : "Fibonacci clock";
  if($("#howtoDlg").open)$("#howtoBody").innerHTML=howtoHtml();
  if(st.synced){
    const p=n=>String(n).padStart(2,"0");
    $("#bigtime").textContent=p(st.hour)+":"+p(st.minute)+":"+p(st.second);
    const days=["Sun","Mon","Tue","Wed","Thu","Fri","Sat"];
    $("#bigdate").textContent=(days[st.wday]||"")+" "+st.day+"/"+st.mon+"/"+st.year;
    const hv=maskVals(st.hourMask),mv=maskVals(st.minMask);
    const hs=hv.length?hv.join(" + "):"0";
    const ms=mv.length?mv.join(" + ")+" ("+(st.minute/5)*5+" min)":"0 min";
    $("#rep").textContent="hour "+hs+"  •  minute "+ms;
  }else{
    $("#bigtime").textContent="--:--";
    $("#bigdate").textContent="waiting for NTP sync";
    $("#rep").textContent="";
  }
  $("#syncdot").className="dot"+(st.synced?" ok":"");
  $("#synctext").textContent=st.synced?"NTP synced":"no time";
  $("#hostline").textContent=(st.name||"fibonacci-clock")+".local  •  "+(st.ip||"offline");
  $("#on").checked=!!st.on;
  if(document.activeElement!==$("#bright"))$("#bright").value=st.brightness??0;
  const sel=$("#tz");
  if(sel.value!==st.tz){
    let opt=[...sel.options].find(o=>o.value===st.tz);
    if(!opt&&st.tz){opt=document.createElement("option");opt.value=st.tz;opt.textContent="Custom ("+st.tz+")";sel.appendChild(opt);}
    sel.value=st.tz||"";
  }
  if(st.synced&&document.activeElement!==$("#manual"))
    $("#manual").value=String(st.hour).padStart(2,"0")+":"+String(st.minute).padStart(2,"0");
  document.querySelectorAll(".theme").forEach((el,i)=>el.classList.toggle("sel",i===st.theme));
  const s=$("#stats");
  const up=st.uptime||0;
  s.innerHTML=
    "<div><b>IP</b><span>"+(st.ip||"-")+"</span></div>"+
    "<div><b>Signal</b><span>"+(st.rssi||0)+" dBm</span></div>"+
    "<div><b>Uptime</b><span>"+Math.floor(up/3600)+"h "+Math.floor(up%3600/60)+"m</span></div>"+
    "<div><b>Free heap</b><span>"+Math.round((st.heap||0)/1024)+" KB</span></div>"+
    "<div><b>Firmware</b><span>"+(st.fw||"-")+"</span></div>"+
    "<div><b>Theme</b><span>"+(st.themeName||"-")+"</span></div>";
}

function buildThemes(){
  const box=$("#themes");box.innerHTML="";
  themes.forEach((t,i)=>{
    const b=document.createElement("button");b.className="theme";b.type="button";
    b.innerHTML='<div class="bar"><i style="background:'+t.hour+'"></i><i style="background:'+t.minute+
      '"></i><i style="background:'+t.both+'"></i></div>'+t.name;
    b.onclick=()=>postConfig({theme:i});
    box.appendChild(b);
  });
}

async function postConfig(o){
  try{
    const r=await fetch("/api/config",{method:"POST",headers:{"Content-Type":"application/json"},body:JSON.stringify(o)});
    const d=await r.json();
    if(!r.ok){toast(d.error||"Save failed");return;}
    state=d;
    if(o.seg)segDirty=false;
    render();toast("Saved");
  }catch(e){toast("Save failed");}
}
function openHowto(){$("#howtoBody").innerHTML=howtoHtml();const d=$("#howtoDlg");if(!d.open)d.showModal();}

let hashOpened=false;
async function load(){
  try{
    const r=await fetch("/api/state");state=await r.json();
    if(!themes.length){
      const tr=await fetch("/api/themes");themes=await tr.json();buildThemes();
    }
    render();
    if(!hashOpened&&location.hash==="#howto"){hashOpened=true;openHowto();}
  }catch(e){$("#synctext").textContent="offline";$("#syncdot").className="dot";}
}
window.addEventListener("hashchange",()=>{if(location.hash==="#howto")openHowto();});

$("#on").addEventListener("change",e=>postConfig({on:e.target.checked}));
$("#bright").addEventListener("input",e=>{
  state.brightness=+e.target.value;render();
});
$("#bright").addEventListener("change",e=>postConfig({brightness:+e.target.value}));
$("#tz").addEventListener("change",e=>postConfig({tz:e.target.value}));
$("#sett").onclick=async()=>{
  const v=$("#manual").value;if(!v){toast("Pick a time");return;}
  const [h,m]=v.split(":").map(Number);
  try{
    const r=await fetch("/api/time",{method:"POST",headers:{"Content-Type":"application/json"},
      body:JSON.stringify({hour:h,min:m})});
    const d=await r.json();
    if(!r.ok){toast(d.error||"Failed");return;}
    state=d;render();toast("Clock set");
  }catch(e){toast("Failed");}
};
$("#reboot").onclick=async()=>{if(!confirm("Reboot the clock?"))return;
  try{await fetch("/api/reboot",{method:"POST"});}catch(e){}toast("Rebooting…");};
$("#wifi").onclick=async()=>{if(!confirm("Erase WiFi settings and restart into the config portal?"))return;
  try{await fetch("/api/wifi/reset",{method:"POST"});}catch(e){}toast("Restarting…");};
$("#saveseg").onclick=()=>{
  const vals=[...document.querySelectorAll("#segs input")].map(i=>parseInt(i.value,10));
  if(vals.some(v=>!Number.isFinite(v)||v<1)){toast("Each segment needs 1 or more LEDs");return;}
  postConfig({seg:vals});
};
$("#testseg").onclick=async()=>{
  try{await fetch("/api/test?mode=segments",{method:"POST"});toast("Testing segments");}catch(e){toast("Failed");}
};
$("#boottest").addEventListener("change",e=>postConfig({bootTest:e.target.checked}));
$("#howto").onclick=openHowto;
$("#howtoClose").onclick=()=>$("#howtoDlg").close();
$("#howtoDlg").addEventListener("click",e=>{if(e.target===$("#howtoDlg"))$("#howtoDlg").close();});

buildSegInputs();
buildTzSelect();
buildFace();
load();
setInterval(load,1000);
</script>
</body>
</html>
)HTML";
