#!/usr/bin/env python

from ROOT import TF1
from ROOT import TCanvas
from ROOT import gROOT
from ROOT import TLine
import math
import ROOT
import AtlasStyle, AtlasUtil
from PyROOTUtils import Legend
from PyROOTUtils import DrawText 

gROOT.SetBatch( True )
v = 246
mquark = [ 0.0023, 0.0048, 0.095, 1.3, 173.34, 4.8 ]
mtb = [4.8, 173.34]
alpha_s = 0.11

def scalar_gg(xx):
    mMed = xx
    w = 0.0
    real = 0.0
    img = 0.0
    for mq in mquark:
        r, i = scalar_factor(4*mq*mq/xx/xx)
        r *=  mq*mq
        i *= mq*mq
        real += r
        img += i
    w = gSM*gSM*alpha_s*alpha_s*(real*real+img*img)/ (2*math.pi*math.pi*math.pi*v*v*mMed)
    return w

def set_f1(f1, title, color):
    f1.SetName(title)
    f1.SetTitle(title)
    f1.GetXaxis().SetTitle("m_{MED} [GeV]")
    f1.GetYaxis().SetTitle("#Gamma_{MED}^{min} [GeV]")
    if do_twice and not do_vector:
        f1.GetYaxis().SetTitle("#Gamma_{MED}^{min}+#Gamma_{t#bar{t}} [GeV]")
    f1.SetLineColor(color)
    
def find_y_equal_x(f1):
    max_value = f1.GetXaxis().GetXmax()
    min_value = f1.GetXaxis().GetXmin()
    nbins = f1.GetXaxis().GetNbins()
    step = (max_value - min_value)/nbins
    p_x = -1
    p_y = -1
    for index in range(nbins):
        v1 = min_value + index*step
        y1 = f1.Eval(v1)
        v2 = min_value + (index+1)*step
        y2 = f1.Eval(v2)
        if (y1-v1)*(y2-v2) > 0:
            p_x = (v1+v2)/2.
            p_y = (y1+y2)/2.
            break
    return (p_x,p_y)

def plot_functions(hists, out_name, do_log):
    #print "total hists: ",len(hists)
    high = []
    low = []
    for hist in hists:
        high.append(hist.GetMaximum())
        low.append(hist.GetMinimum())
    max_y = max(high) * 1.1
    min_y = min(low)
    canvas = TCanvas("canvas", "", 600, 600)
    if do_log:
        min_y = 1e-3
        max_y = 1e4
        canvas.SetLogy()
        #max_y *= 10
        #if min_y == 0:
        #    min_y = 1e-5
    hists[0].GetYaxis().SetRangeUser(min_y, max_y)
    hists[0].GetXaxis().SetNdivisions(8)
    hists[0].GetXaxis().SetLabelSize(0.03)
    hists[0].GetYaxis().SetLabelSize(0.03)
    leg = Legend( 0.65, 0.650, textSize=0.03 )
    for index in range(len(hists)):
        if index == 0:
            hists[index].Draw()
        else:
            hists[index].Draw('same')
        #(p_x,p_y) = find_y_equal_x(hists[index])
        #if p_x > 0:
        #    line1 = TLine(p_x,0, p_x, p_y)
        #    line1.SetLineColor(hists[index].GetLineColor())
        #    line1.Draw('same')
        #else:
        #    #print "no intersection"
        if hists[index].GetName().find("chi") == -1 and \
           hists[index].GetName().find("Gamma") == -1:
            leg.AddEntry(hists[index].GetName(),hists[index].GetName(),"L")
    leg.Draw()
    DrawText(0.2,0.86,out_name+" Mediator, g_{DM} = "+str(gDM)+\
             ", g_{SM} = "+str(gSM),textSize=0.035)
    if do_twice:
        out_name = out_name + "_2tops"
    if do_log:
        canvas.SaveAs(("%s_%.2f_%.2f_Log.eps")%(out_name,gSM,gDM))
        canvas.SaveAs(("%s_%.2f_%.2f_Log.png")%(out_name,gSM,gDM))
    else:
        canvas.SaveAs(("%s_%.2f_%.2f.eps")%(out_name,gSM,gDM))
        canvas.SaveAs(("%s_%.2f_%.2f.png")%(out_name,gSM,gDM))

def get_vector_expr(mf, gf):
    return str(gf)+"^2*(x^2 + 2*"+str(mf)+"^2)*sqrt(1 - 4*"+\
            str(mf)+"^2/x^2)/(12*TMath::Pi()*x)"
    title = "m_{DM} = "+str(mf)+", g_{f} = "+str(gf)
    print title
    output = "vector_"+str(mf)+"_"+str(gf)
    return [vector_mediator, title, output]

def get_axial_vector_expr(mf, gf):
    return str(gf)+"^2*(x^2 - 4*"+str(mf)+"^2)*sqrt(1 - 4*"+\
            str(mf)+"^2/x^2)/(12*TMath::Pi()*x)"

def get_scalar_expr(mf, gf):
    #return str(gf)+"^2*"+str(mf)+"^2*x*TMath::Power((1- 4*"+\
    #        str(mf)+"^2/x^2),1.5)/(8*TMath::Pi()*246^2)"
    return str(gf)+"^2*"+str(mf)+"^2*x*sqrt(1- 4*"+\
             str(mf)+"^2/x^2)*(1-4*"+str(mf)+"^2/x^2)/(8*TMath::Pi()*246^2) + 1e-5"

def get_pseudo_scalar_expr(mf, gf):
    return str(gf)+"^2*"+str(mf)+"^2*x*sqrt(1- 4*"+\
            str(mf)+"^2/x^2)/(8*TMath::Pi()*246^2)"

def get_scalar_dm(mf, gf):
    return str(gf)+"^2*x*TMath::Power((1- 4*"+\
            str(mf)+"^2/x^2),1.5)/(8*TMath::Pi())"

def get_pseudo_scalar_dm(mf, gf):
    return str(gf)+"^2*x*sqrt(1- 4*"+\
            str(mf)+"^2/x^2)/(8*TMath::Pi())"

def scalar_dm(mMed, mDM):
    w = 0.0
    if mMed > 2*mDM:
        w = w + gDM*gDM/8./math.pi * mMed * math.sqrt(1 - 4*mDM*mDM/mMed/mMed) \
                * (1 - 4*mDM*mDM/mMed/mMed)
    return w

def scalar_sm_10(x): # dm + sm
    xx = x[0]
    mMed = xx
    w = 0.0
    w += scalar_dm(xx, 10)
    w += scalar_gg(xx)
    for mq in mquark:
        if xx > 2*mq:
            w += 3*(gSM*gSM/8./math.pi/v/v*mq*mq*mMed \
                       * math.sqrt(1 - 4*mq*mq/mMed/mMed) \
                       * (1 - 4*mq*mq/mMed/mMed))
    return w + scalar_gg(xx)

def scalar_sm_30(x):
    xx = x[0]
    mMed = xx
    w = 0.0
    w += scalar_dm(xx, 30)
    w += scalar_gg(xx)
    for mq in mquark:
        if xx > 2*mq:
            w = w + 3*(gSM*gSM/8./math.pi/v/v*mq*mq*mMed \
                       * math.sqrt(1 - 4*mq*mq/mMed/mMed) \
                       * (1 - 4*mq*mq/mMed/mMed))
    return w

def scalar_sm_100(x):
    xx = x[0]
    mMed = xx
    w = 0.0
    w += scalar_dm(xx, 100)
    w += scalar_gg(xx)
    for mq in mquark:
        if xx > 2*mq:
            w = w + 3*(gSM*gSM/8./math.pi/v/v*mq*mq*mMed \
                       * math.sqrt(1 - 4*mq*mq/mMed/mMed) \
                       * (1 - 4*mq*mq/mMed/mMed))
    return w

def scalar_sm_300(x):
    xx = x[0]
    mMed = xx
    w = 0.0
    w += scalar_dm(xx, 300)
    w += scalar_gg(xx)

    for mq in mquark:
        if xx > 2*mq:
            w = w + 3*(gSM*gSM/8./math.pi/v/v*mq*mq*mMed \
                       * math.sqrt(1 - 4*mq*mq/mMed/mMed) \
                       * (1 - 4*mq*mq/mMed/mMed))
    return w

def scalar_dm_10(x):
    xx = x[0]
    return scalar_dm(xx, 10)

def scalar_dm_30(x):
    xx = x[0]
    return scalar_dm(xx, 30)

def scalar_dm_100(x):
    xx = x[0]
    return scalar_dm(xx, 100)

def scalar_dm_300(x):
    xx = x[0]
    return scalar_dm(xx, 300)

def actan_square(x):
    if x - 1 > 1e-3:
        real = math.atan(1/math.sqrt(x-1))*math.atan(1/math.sqrt(x-1))
        img = 0.0
        return (real, img)
    elif 1 - x > 1e-3:
        y = -1/math.sqrt(1-x)
        if abs(y -1) < 1e-3:
            return (0,0)
        img = (-0.5)*(0.5*math.log((1-y*y)*(1-y*y)) - math.log((1+y)*(1+y)))
        if (y*y < 1):
            real = 0
        else:
            real = (-0.5)*math.pi
        # print real, img
        real2 = real*real - img*img
        img2 = 2*real*img
        return (real2, img2)
    else:
        return (1., 0)

def scalar_factor(x):
    real, img = actan_square(x)
    return (1+(1-x)*real, (1-x)*img)


def scalar_gg_func(x):
    xx = x[0]
    return scalar_gg(xx)

def pseudo_factor(x):
    return actan_square(x)

def pseudo_gg_func(x):
    xx = x[0]
    return pseudo_gg(xx)

def pseudo_gg(xx):
    mMed = xx
    real = 0.0
    img = 0.0
    for mq in mquark:
        r, i = pseudo_factor(4*mq*mq/xx/xx)
        r *= mq*mq
        i *= mq*mq
        real += r
        img += i
    w = gSM*gSM*alpha_s*alpha_s*(real*real + img*img)/ (2*math.pi*math.pi*math.pi*v*v*mMed)
    return w

def pseudo_dm(mMed, mDM):
    w = 0.0
    if mMed > 2*mDM:
        w = w + gDM*gDM/8./math.pi * mMed * math.sqrt(1 - 4*mDM*mDM/mMed/mMed)
    return w

def pseudo_sm_10(x):
    xx = x[0]
    mMed = xx
    w = 0.0
    w += pseudo_dm(mMed, 10)
    w += pseudo_gg(xx)
    for mq in mquark:
        if xx > 2*mq :
            w = w + 3*(gSM*gSM/8./math.pi/v/v*mq*mq*mMed * \
                       math.sqrt(1 - 4*mq*mq/mMed/mMed))
    return w

def pseudo_sm_30(x):
    xx = x[0]
    mMed = xx
    w = 0.0
    w += pseudo_dm(mMed, 30)
    w += pseudo_gg(xx)
    for mq in mquark:
        if xx > 2*mq:
            w = w + 3*(gSM*gSM/8./math.pi/v/v*mq*mq*mMed \
                       * math.sqrt(1 - 4*mq*mq/mMed/mMed))
    return w

def pseudo_sm_100(x):
    xx = x[0]
    mMed = xx
    w = 0.0
    w += pseudo_dm(mMed, 100)
    w += pseudo_gg(xx)
    for mq in mquark:
        if xx > 2*mq:
            w = w + 3*(gSM*gSM/8./math.pi/v/v*mq*mq*mMed \
                       * math.sqrt(1 - 4*mq*mq/mMed/mMed))
    return w

def pseudo_sm_300(x):
    xx = x[0]
    mMed = xx
    w = 0.0
    mDM = 300
    w += pseudo_dm(mMed, 300)
    w += pseudo_gg(xx)
    for mq in mquark:
        if xx > 2*mq:
            w = w + 3*(gSM*gSM/8./math.pi/v/v*mq*mq*mMed \
                       * math.sqrt(1 - 4*mq*mq/mMed/mMed))
    return w

def pseudo_dm_10(x):
    xx = x[0]
    return pseudo_dm(xx, 10)

def pseudo_dm_30(x):
    xx = x[0]
    return pseudo_dm(xx, 30)

def pseudo_dm_100(x):
    xx = x[0]
    return pseudo_dm(xx, 100)

def pseudo_dm_300(x):
    xx = x[0]
    return pseudo_dm(xx, 300)

def process_once(do_vector, do_twice, gSM, gDM):
    m_chi_array = [10, 30, 100, 300]
    m_top = 173.21
    colors = [1, 2, 28, 3, 4, 6, 41, 9, 41, 45]
    xmin = 0.2
    xmax = 2000
    work_dir ={}
    dm_dir = {}
    if do_vector:
        work_dir["Vector"] = get_vector_expr
        work_dir["Axial"] = get_axial_vector_expr
        dm_dir["Vector"] = get_vector_expr
        dm_dir["Axial"] = get_axial_vector_expr
    else:
        work_dir["Scalar"] = get_scalar_expr
        work_dir["Pseudo_Scalar"] = get_pseudo_scalar_expr
        dm_dir["Scalar"] = get_scalar_dm
        dm_dir["Pseudo_Scalar"] = get_pseudo_scalar_dm
    for key in work_dir.keys():
        hists_vec = []
        index = 0
        for m_chi in m_chi_array:
            # xmin = 2*m_chi
            if key.find("Pseudo_Scalar") != -1:
                #n_top_contribute = 1
                #if do_twice : 
                #    n_top_contribute = 2
                #f1 = TF1("f1",dm_dir[key](m_chi,gDM)+"+3*"+\
                #         str(n_top_contribute)+"*"+\
                #         work_dir[key](m_top,gSM)+"+9*"+\
                #         work_dir[key](0.0, gSM)+"+3*"+\
                #         work_dir[key](1.275, gSM)+"+3*"+\
                #         work_dir[key](4.18, gSM), xmin, xmax)
                if m_chi == 10:
                    f1 = TF1("f1_"+str(m_chi), pseudo_sm_10, xmin, xmax)
                    f11 = TF1("f11_"+str(m_chi), pseudo_dm_10, xmin, xmax)
                elif m_chi == 100:
                    f1 = TF1("f1_"+str(m_chi), pseudo_sm_100, xmin, xmax)
                    f11 = TF1("f11_"+str(m_chi), pseudo_dm_100, xmin, xmax)
                elif m_chi == 30:
                    f1 = TF1("f1_"+str(m_chi), pseudo_sm_30, xmin, xmax)
                    f11 = TF1("f11_"+str(m_chi), pseudo_dm_30, xmin, xmax)
                elif m_chi == 300:
                    f1 = TF1("f1_"+str(m_chi), pseudo_sm_300, xmin, xmax)
                    f11 = TF1("f11_"+str(m_chi), pseudo_dm_300, xmin, xmax)
                else:
                    pass
            elif key.find("Scalar") != -1:
                if m_chi == 10:
                    f1 = TF1("f1_"+str(m_chi), scalar_sm_10, xmin, xmax)
                    f11 = TF1("f11_"+str(m_chi), scalar_dm_10, xmin, xmax)
                elif m_chi == 100:
                    f1 = TF1("f1_"+str(m_chi), scalar_sm_100, xmin, xmax)
                    f11 = TF1("f11_"+str(m_chi), scalar_dm_100, xmin, xmax)
                elif m_chi == 30:
                    f1 = TF1("f1_"+str(m_chi), scalar_sm_30, xmin, xmax)
                    f11 = TF1("f11_"+str(m_chi), scalar_dm_30, xmin, xmax)
                elif m_chi == 300:
                    f1 = TF1("f1_"+str(m_chi), scalar_sm_300, xmin, xmax)
                    f11 = TF1("f11_"+str(m_chi), scalar_dm_300, xmin, xmax)
                else:
                    pass
            else:
                f1 = TF1("f1",work_dir[key](0.0,gSM)+"*9 + 3*"+\
                         work_dir[key](1.275,gSM)+"+3*"+\
                         work_dir[key](4.18, gSM)+"+"+\
                         dm_dir[key](m_chi,gDM)+"+3*"+\
                         work_dir[key](m_top,gSM), xmin,xmax)
            title = "m_{DM} = "+str(m_chi)+" GeV"
            set_f1(f1, title, colors[index])
            hists_vec.append(f1)
            #add chichi's contribution
            # if key.find("Scalar") != -1 and key.find("Pseudo_Scalar") == -1:
            if key.find("Scalar") != -1:
                pass
            else:
                f11 = TF1("f1_chi_"+str(m_chi),dm_dir[key](m_chi,gDM),xmin,xmax)
            set_f1(f11, "#chi#chi, #chi="+str(m_chi), colors[index])
            f11.SetLineStyle(2)
            hists_vec.append(f11)
            index += 1

        #add ttbar's contribution
        # xmin = 2*m_top
        f1 = TF1("f1","3*"+work_dir[key](m_top,gSM),xmin,xmax)
        set_f1(f1, "t#bar{t}", colors[index])
        f1.SetLineStyle(2)
        index += 1
        hists_vec.append(f1)
        if True:
            xmin = 4.2*2
            f2 = TF1("f2",work_dir[key](0.0,gSM)+"*9 + 3*"+\
                    work_dir[key](1.3,gSM)+"+3*"+\
                    work_dir[key](4.8, gSM), xmin,xmax)
            title = "q#bar{q}"
            set_f1(f2, title, colors[index])
            f2.SetLineStyle(2)
            hists_vec.append(f2)
            index += 1
        
        ## add gg for Scalar
        if key.find("Scalar") != -1:
            if key.find("Pseudo_Scalar") != -1:
                f12 = TF1("f12_gg", pseudo_gg_func, xmin, xmax)
            else:
                f12 = TF1("f12_gg", scalar_gg_func, xmin, xmax)
            set_f1(f12, "gg", colors[index])
            f12.SetLineStyle(2)
            hists_vec.append(f12)

        f1 = TF1("f1","x",0, xmax)
        f1.SetLineWidth(1)
        f1.SetLineStyle(3)
        set_f1(f1, "#Gamma == m_{MED}", 1)
        hists_vec.append(f1)
            
        plot_functions(hists_vec,key,True)
        #plot_functions(hists_vec,key,False)
        del hists_vec[:]

def plot_all():
    global do_vector
    global do_twice
    global gDM
    global gSM
    do_vector = False
    do_twice = True 
    #gSM = 1.0
    #gDM_list = [1.0, 2.0, 4.0]
    #for gDM in gDM_list:
    #    process_once(do_vector, do_twice, gSM, gDM)

    do_twice = False
    gDM_list = [1.0, 2.0]
    gSM_list = [2.0, 1.0]
    for gDM in gDM_list:
        for gSM in gSM_list:
            process_once(do_vector, do_twice, gSM, gDM)

    #do_twice = False 
    #for gDM in gDM_list:
    #    process_once(do_vector, do_twice, gSM, gDM)

    do_vector = True
    g_list = [0.5, 1.0, 1.45]
    for gDM in g_list:
        gSM = gDM
        process_once(do_vector, do_twice, gSM, gDM)

    gSM = 1.0
    gDM = 0.25
    process_once(do_vector, do_twice, gSM, gDM)

if __name__ == '__main__':
    global do_vector
    global do_twice
    global gDM
    global gSM
    gSM = 1.0
    gDM = 1.0
    do_vector = False
    do_twice = False
    process_once(do_vector, do_twice, gSM, gDM)
    #print scalar_gg(173.34)
    #print scalar_factor(0.44)

    # do_vector = False
    # process_once(do_vector, do_twice, gSM, gDM)
