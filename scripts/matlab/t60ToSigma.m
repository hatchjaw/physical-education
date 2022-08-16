function [sigma0, sigma1] = t60ToSigma(c, kappa, f1, t60_1, f2, t60_2)
%T60TOSIGMA from Bilbao, p. 178 & p. 400
    cSq = c^2;
    cSqSq = cSq^2;
    kappaSq = kappa^2;
    sixLogTen = 6*log(10);
    
    if kappa == 0
        zeta = (2*pi*f2)^2/cSq;
        sigma0 = sixLogTen/t60_1;
        sigma1  = (sixLogTen*(1/t60_2 - 1/t60_1))/zeta;
    else
        if c == 0
            zeta1 = 2*pi*f1/kappa;
            zeta2 = 2*pi*f2/kappa;
        else
            zeta1 = (-cSq + sqrt(cSqSq + 4*kappaSq*(2*pi*f1)^2))/(2*kappaSq); 
            zeta2 = (-cSq + sqrt(cSqSq + 4*kappaSq*(2*pi*f2)^2))/(2*kappaSq);
        end
        
        sigma0 = sixLogTen*(zeta1/t60_2 - zeta2/t60_1)/(zeta1-zeta2); 
        sigma1 = sixLogTen*(1/t60_1 - 1/t60_2)/(zeta1-zeta2);
    end
    
    

    % % From Craig...
    % sigma0 = (6*log(10))/t60_1;
    
    % if (kappa == 0)
    %     zeta2 = (2*pi*omega2)^2/(c^2);
    % end

    % if (c == 0)
    %     zeta2 = 2*pi*omega2/kappa;
    % end

    % sigma1  = (6*log(10)*(1/t60_2 - 1/t60_1))/zeta2;
end